/*
 * This file is part of lslidar_n301 driver.
 *
 * The driver is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The driver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the driver.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <boost/thread.hpp>

#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>

#include <lslidar_n301_driver/lslidar_n301_driver_nodelet.h>


namespace lslidar_n301_driver
{

LslidarN301DriverNodelet::LslidarN301DriverNodelet():
  running(false) {
  return;
}

LslidarN301DriverNodelet::~LslidarN301DriverNodelet() {
  if (running) {
    NODELET_INFO("shutting down driver thread");
    running = false;
    device_thread->join();
    NODELET_INFO("driver thread stopped");
  }
  return;
}

void LslidarN301DriverNodelet::onInit()
{
  // start the driver
  lslidar_n301_driver.reset(
      new LslidarN301Driver(getNodeHandle(), getPrivateNodeHandle()));
  if (!lslidar_n301_driver->initialize()) {
    ROS_ERROR("Cannot initialize lslidar driver...");
    return;
  }

  // spawn device poll thread
  running = true;
  device_thread = boost::shared_ptr< boost::thread >
    (new boost::thread(boost::bind(&LslidarN301DriverNodelet::devicePoll, this)));
}

/** @brief Device poll thread main loop. */
void LslidarN301DriverNodelet::devicePoll()
{
  while(ros::ok()) {
    // poll device until end of file
    running = lslidar_n301_driver->polling();
    if (!running)
      break;
  }
  running = false;
}

} // namespace lslidar_driver

// Register this plugin with pluginlib.  Names must match nodelet_lslidar.xml.
//
// parameters are: package, class name, class type, base class type
PLUGINLIB_DECLARE_CLASS(lslidar_n301_driver, LslidarN301DriverNodelet,
                        lslidar_n301_driver::LslidarN301DriverNodelet, nodelet::Nodelet);
