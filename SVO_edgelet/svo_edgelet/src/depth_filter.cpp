// This file is part of SVO - Semi-direct Visual Odometry.
//
// Copyright (C) 2014 Christian Forster <forster at ifi dot uzh dot ch>
// (Robotics and Perception Group, University of Zurich, Switzerland).
//
// SVO is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or any later version.
//
// SVO is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <algorithm>
#include <boost/bind.hpp>
#include <boost/math/distributions/normal.hpp>
#include <svo/global.h>
#include <svo/depth_filter.h>
#include <svo/frame.h>
#include <svo/point.h>
#include <svo/feature.h>
#include <svo/matcher.h>
#include <svo/config.h>
#include <svo/feature_detection.h>
#include <sys/time.h>

namespace svo {

int Seed::batch_counter = 0;
int Seed::seed_counter = 0;

Seed::Seed(Feature* ftr, float depth_mean, float depth_min) :
    batch_id(batch_counter),
    id(seed_counter++),
    ftr(ftr),
    a(10),
    b(10),
    mu(1.0/depth_mean),
    z_range(1.0/depth_min),
    sigma2(z_range*z_range/36)  // 36
{}

DepthFilter::DepthFilter(feature_detection::DetectorPtr feature_detector, feature_detection::DetectorPtr edge_detector, callback_t seed_converged_cb) :
    feature_detector_(feature_detector),
    edge_detector_(edge_detector),
    seed_converged_cb_(seed_converged_cb),
    seeds_updating_halt_(false),
    thread_(NULL),
    new_keyframe_set_(false),
    new_keyframe_min_depth_(0.0),
    new_keyframe_mean_depth_(0.0)
{}

DepthFilter::~DepthFilter()
{
  stopThread();
  SVO_INFO_STREAM("DepthFilter destructed.");
}

void DepthFilter::startThread()
{
  thread_ = new boost::thread(&DepthFilter::updateSeedsLoop, this);
}

void DepthFilter::stopThread()
{
  SVO_INFO_STREAM("DepthFilter stop thread invoked.");
  if(thread_ != NULL)
  {
    SVO_INFO_STREAM("DepthFilter interrupt and join thread... ");
    seeds_updating_halt_ = true;
    thread_->interrupt();
    thread_->join();
    thread_ = NULL;
  }
}

void DepthFilter::addFrame(FramePtr frame)
{
  if(thread_ != NULL)
  {
    {
      lock_t lock(frame_queue_mut_);
      if(frame_queue_.size() > 5)
        frame_queue_.pop();
      if (frame)
      frame_queue_.push(frame);
    }
    seeds_updating_halt_ = false;
    frame_queue_cond_.notify_one();
  }
  else
    updateSeeds(frame);
}

void DepthFilter::addKeyframe(FramePtr frame, double depth_mean, double depth_min)
{
  new_keyframe_min_depth_ = depth_min;
  new_keyframe_mean_depth_ = depth_mean;
  if(thread_ != NULL)
  {
    new_keyframe_ = frame;
    new_keyframe_set_ = true;
    seeds_updating_halt_ = true;
    frame_queue_cond_.notify_one();
  }
  else
    initializeSeeds(frame);
}

void DepthFilter::initializeSeeds(FramePtr frame)
{

  if(frame->have_initializeSeeds)
    return;

  Features new_features;
  feature_detector_->setExistingFeatures(frame->fts_);
  feature_detector_->detect(frame.get(), frame->img_pyr_,
                            Config::triangMinCornerScore(), new_features);

  edge_detector_->detect(frame.get(), frame->img_pyr_,
                         Config::triangMinCornerScore(), new_features);

  // initialize a seed for every new feature
  seeds_updating_halt_ = true;
  lock_t lock(seeds_mut_); // by locking the updateSeeds function stops
  ++Seed::batch_counter;

  //SVO_DEBUG_STREAM("67676767676.");
  std::for_each(new_features.begin(), new_features.end(), [&](Feature* ftr){
    //seeds_.push_back(Seed(ftr, new_keyframe_mean_depth_, new_keyframe_min_depth_));

    // hyj  fix code : useing neighbouring fts to init the seeds depth
    double dist_min(100), z(new_keyframe_mean_depth_);
    for(auto it=frame->fts_.begin(), ite=frame->fts_.end(); it!=ite; ++it)
    {
      if((*it)->point != NULL)
      {
        Vector2d dist= ( ftr->px - (*it)->px);
        if(dist.norm() < dist_min )
        {
          dist_min = dist.norm();
          z = frame->w2f((*it)->point->pos_).z();
        }
      }
    }
    if(dist_min < 70)
      seeds_.push_back(Seed(ftr, z, new_keyframe_min_depth_));
    else
      seeds_.push_back(Seed(ftr, new_keyframe_mean_depth_, new_keyframe_min_depth_));

  });

  //SVO_DEBUG_STREAM("68686868686.");
  if(options_.verbose)
    SVO_INFO_STREAM("DepthFilter: Initialized "<<new_features.size()<<" new seeds");
  seeds_updating_halt_ = false;

  frame->have_initializeSeeds = true;

}

void DepthFilter::removeKeyframe(FramePtr frame)
{

  seeds_updating_halt_ = true;
  lock_t lock(seeds_mut_);
  list<Seed>::iterator it=seeds_.begin();
  size_t n_removed = 0;
  //SVO_DEBUG_STREAM("6666666666666666.");
  while(it!=seeds_.end())
  {
    if(it->ftr->frame == frame.get())
    {
      it = seeds_.erase(it);
      ++n_removed;
    }
    else
      ++it;
  }
   // SVO_DEBUG_STREAM("77777777777.");
  seeds_updating_halt_ = false;
}

void DepthFilter::reset()
{
  seeds_updating_halt_ = true;
  {
    lock_t lock(seeds_mut_);
    seeds_.clear();
  }
  lock_t lock();
  while(!frame_queue_.empty())
    frame_queue_.pop();
  seeds_updating_halt_ = false;

  if(options_.verbose)
    SVO_INFO_STREAM("DepthFilter: RESET.");
}

void DepthFilter::updateSeedsLoop()
{
  while(!boost::this_thread::interruption_requested())
  {
    FramePtr frame;
    {
      lock_t lock(frame_queue_mut_);
      while(frame_queue_.empty() && new_keyframe_set_ == false)
        frame_queue_cond_.wait(lock);


      if(new_keyframe_set_)
      {
        new_keyframe_set_ = false;
        seeds_updating_halt_ = false;
       clearFrameQueue();
        frame = new_keyframe_;
      }
      else
      {
        frame = frame_queue_.front();
        frame_queue_.pop();
      }
    }
    struct  timeval    tv0,tv1;
    struct  timezone   tz;
     
    gettimeofday(&tv0,&tz);
    updateSeeds(frame);
    if(frame->isKeyframe())
    {
      struct  timeval    tv2,tv3;
      struct  timezone   tz1;
       
      gettimeofday(&tv2,&tz1);
      initializeSeeds(frame);
      gettimeofday(&tv3,&tz1);
      printf("-------------------------keyframe init time : %d ms----------------\n",(tv3.tv_sec*1000+tv3.tv_usec/1000)-(tv2.tv_sec*1000+tv2.tv_usec/1000));
  
    }
      
    gettimeofday(&tv1,&tz);
    // printf("-------------------------keyframe time : %d ms----------------\n",(tv1.tv_sec*1000+tv1.tv_usec/1000)-(tv0.tv_sec*1000+tv0.tv_usec/1000));

  }
}

void DepthFilter::updateSeeds(FramePtr frame)
{
  // update only a limited number of seeds, because we don't have time to do it
  // for all the seeds in every frame!
  size_t n_updates=0, n_failed_matches=0, n_seeds = seeds_.size();
  lock_t lock(seeds_mut_);
  list<Seed>::iterator it=seeds_.begin();

  const double focal_length = frame->cam_->errorMultiplier2();
  double px_noise = 0.5;
  double px_error_angle = atan(px_noise/(2.0*focal_length))*2.0; // law of chord (sehnensatz)

  //SVO_DEBUG_STREAM("55555555555555.");
  int erase_seed = 0;
  while( it!=seeds_.end())
  {
    // set this value true when seeds updating should be interrupted
    if(seeds_updating_halt_)
      return;

    // check if seed is not already too old
    if((Seed::batch_counter - it->batch_id) > options_.max_n_kfs) {
    // if((Seed::batch_counter - it->batch_id) > 3) {
      it = seeds_.erase(it);
      continue;
    }
/*
    if(( it->sigma2 > (it->z_range*it->z_range)/25 ))// || ( (it->mu - sqrt(it->sigma2))<0 ) )
    {
      //std::cout<<"sigma2 is larger than z_range"<<std::endl;
      it = seeds_.erase(it);
      erase_seed ++;
      continue;
    }
*/
    // check if point is visible in the current image
    SE3 T_ref_cur = it->ftr->frame->T_f_w_ * frame->T_f_w_.inverse();
    const Vector3d xyz_f(T_ref_cur.inverse()*(1.0/it->mu * it->ftr->f) );
    if(xyz_f.z() < 0.0)  {
      ++it; // behind the camera
      continue;
    }
    if(!frame->cam_->isInFrame(frame->f2c(xyz_f).cast<int>())) {
      ++it; // point does not project in image
      continue;
    }

    // we are using inverse depth coordinates
    float sigma =  sqrt(it->sigma2);
    float z_inv_min = it->mu + sigma;
    float z_inv_max = max(it->mu - sigma, 0.00000001f);
    double z;
    double tau_scale = 1;
    if(!matcher_.findEpipolarMatchDirect(
        *it->ftr->frame, *frame, *it->ftr, 1.0/it->mu, 1.0/z_inv_min, 1.0/z_inv_max, z, tau_scale))
    {
      it->b++; // increase outlier probability when no match was found
      ++it;
      ++n_failed_matches;
      continue;
    }

    // compute tau
    double tau = computeTau(T_ref_cur, it->ftr->f, z, px_error_angle * tau_scale);
    double tau_inverse = 0.5 * (1.0/max(0.0000001, z-tau) - 1.0/(z+tau));

    // update the estimate
    updateSeed(1./z, tau_inverse*tau_inverse, &*it);
    ++n_updates;

    if(frame->isKeyframe())
    {
      // The feature detector should not initialize new seeds close to this location
      feature_detector_->setGridOccpuancy(matcher_.px_cur_);
    }


    if(it->ftr->type == Feature::EDGELET)
    {
      it->ftr->grad_cur_ = (matcher_.A_cur_ref_ * it->ftr->grad).normalized();
    }

    // std::vector<Point*> CandPoints;
    // CandPoints.clear();
    // std::vector<double> sigmas;
    // if the seed has converged, we initialize a new candidate point and remove the seed
    if(sqrt(it->sigma2) < it->z_range/options_.seed_convergence_sigma2_thresh)
    {
      assert(it->ftr->point == NULL); // TODO this should not happen anymore
      Vector3d xyz_world(it->ftr->frame->T_f_w_.inverse() * (it->ftr->f * (1.0/it->mu)));

      if(it->ftr->type == Feature::EDGELET)
      {
         it->ftr->grad =  it->ftr->grad_cur_;  //  edgelete in newkeyframe, it's direction should change;
      }

      Point* point = new Point(xyz_world, it->ftr);
      it->ftr->point = point;
      /* FIXME it is not threadsafe to add a feature to the frame here.
      if(frame->isKeyframe())
      {
        Feature* ftr = new Feature(frame.get(), matcher_.px_cur_, matcher_.search_level_);
        ftr->point = point;
        point->addFrameRef(ftr);
        frame->addFeature(ftr);
        it->ftr->frame->addFeature(it->ftr);
      }
      else
      */
      {
        seed_converged_cb_(point, it->sigma2); // put in candidate list
        // CandPoints.push_back(point);
      }
      it = seeds_.erase(it);
    }
    else if(isnan(z_inv_min))
    {
      SVO_WARN_STREAM("z_min is NaN");
      it = seeds_.erase(it);
    }
    else
      ++it;
  
    // if (CandPoints.size() > 0)
    // {
    //   seed_converged_cb_(CandPoints, CandPoints.size());
    // }
  }

  if(erase_seed > 0)
  {
      std::cout << "erase seed : "<<erase_seed<<std::endl;
  }

}

void DepthFilter::clearFrameQueue()
{
  while(!frame_queue_.empty())
    frame_queue_.pop();
}

void DepthFilter::getSeedsCopy(const FramePtr& frame, std::list<Seed>& seeds)
{
  lock_t lock(seeds_mut_);
  for(std::list<Seed>::iterator it=seeds_.begin(); it!=seeds_.end(); ++it)
  {
    if (it->ftr->frame == frame.get())
      seeds.push_back(*it);
  }
}

void DepthFilter::getAllSeedsCopy(std::list<Seed>& seeds)
{
  lock_t lock(seeds_mut_);
  for(std::list<Seed>::iterator it=seeds_.begin(); it!=seeds_.end(); ++it)
  {
      seeds.push_back(*it);
  }
}

void DepthFilter::updateSeed(const float x, const float tau2, Seed* seed)
{
  float norm_scale = sqrt(seed->sigma2 + tau2);
  if(std::isnan(norm_scale))
    return;
  boost::math::normal_distribution<float> nd(seed->mu, norm_scale);
  float s2 = 1./(1./seed->sigma2 + 1./tau2);
  float m = s2*(seed->mu/seed->sigma2 + x/tau2);
  float C1 = seed->a/(seed->a+seed->b) * boost::math::pdf(nd, x);
  float C2 = seed->b/(seed->a+seed->b) * 1./seed->z_range;
  float normalization_constant = C1 + C2;
  C1 /= normalization_constant;
  C2 /= normalization_constant;
  float f = C1*(seed->a+1.)/(seed->a+seed->b+1.) + C2*seed->a/(seed->a+seed->b+1.);
  float e = C1*(seed->a+1.)*(seed->a+2.)/((seed->a+seed->b+1.)*(seed->a+seed->b+2.))
          + C2*seed->a*(seed->a+1.0f)/((seed->a+seed->b+1.0f)*(seed->a+seed->b+2.0f));

  // update parameters
  float mu_new = C1*m+C2*seed->mu;
  seed->sigma2 = C1*(s2 + m*m) + C2*(seed->sigma2 + seed->mu*seed->mu) - mu_new*mu_new;
  seed->mu = mu_new;
  seed->a = (e-f)/(f-e/f);
  seed->b = seed->a*(1.0f-f)/f;
}

double DepthFilter::computeTau(
      const SE3& T_ref_cur,
      const Vector3d& f,
      const double z,
      const double px_error_angle)
{
  Vector3d t(T_ref_cur.translation());
  Vector3d a = f*z-t;
  double t_norm = t.norm();
  double a_norm = a.norm();
  double alpha = acos(f.dot(t)/t_norm); // dot product
  double beta = acos(a.dot(-t)/(t_norm*a_norm)); // dot product
  double beta_plus = beta + px_error_angle;
  double gamma_plus = PI-alpha-beta_plus; // triangle angles sum to PI
  double z_plus = t_norm*sin(beta_plus)/sin(gamma_plus); // law of sines
  return (z_plus - z); // tau
}

} // namespace svo
