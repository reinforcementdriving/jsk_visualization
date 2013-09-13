#include <ros/ros.h>

#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>

#include <interactive_markers/interactive_marker_server.h>
#include <interactive_marker_helpers/interactive_marker_helpers.h>

#include <interactive_markers/menu_handler.h>
#include <jsk_interactive_marker/SetPose.h>
#include <jsk_interactive_marker/MarkerSetPose.h>

#include <math.h>
#include <jsk_interactive_marker/MarkerMenu.h>
#include <jsk_interactive_marker/MarkerPose.h>

#include <std_msgs/Int8.h>

#include <jsk_interactive_marker/interactive_marker_interface.h>


void InteractiveMarkerInterface::proc_feedback( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback ) {
  jsk_interactive_marker::MarkerPose mp;
  mp.pose.header = feedback->header;
  mp.pose.pose = feedback->pose;
  mp.marker_name = feedback->marker_name;
  pub_.publish( mp );
}

void InteractiveMarkerInterface::pub_marker_menuCb(const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback, int menu){
  jsk_interactive_marker::MarkerMenu m;
  m.marker_name = feedback->marker_name;
  m.menu=menu;
  pub_move_.publish(m);
}


void InteractiveMarkerInterface::pub_marker_menu(std::string marker,int menu){
  jsk_interactive_marker::MarkerMenu m;
  m.marker_name = marker;
  m.menu=menu;
  pub_move_.publish(m);
}

void InteractiveMarkerInterface::IMSizeLargeCb( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback ){
  geometry_msgs::PoseStamped pose;
  pose.header = feedback->header;
  pose.pose = feedback->pose;
  changeMarkerMoveMode(feedback->marker_name, 0, 0.5, pose);
}

void InteractiveMarkerInterface::IMSizeMiddleCb( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback ){
  geometry_msgs::PoseStamped pose;
  pose.header = feedback->header;
  pose.pose = feedback->pose;
  changeMarkerMoveMode(feedback->marker_name, 0, 0.3, pose);
}

void InteractiveMarkerInterface::IMSizeSmallCb( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback ){
  geometry_msgs::PoseStamped pose;
  pose.header = feedback->header;
  pose.pose = feedback->pose;
  changeMarkerMoveMode(feedback->marker_name, 0, 0.1, pose);
}

void InteractiveMarkerInterface::changeMoveModeCb( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback ){
  changeMarkerMoveMode(feedback->marker_name,0);
  pub_marker_menu(feedback->marker_name,13);

}
void InteractiveMarkerInterface::changeMoveModeCb1( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback ){
  changeMarkerMoveMode(feedback->marker_name,1);
  pub_marker_menu(feedback->marker_name,13);

}
void InteractiveMarkerInterface::changeMoveModeCb2( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback ){
  changeMarkerMoveMode(feedback->marker_name,2);
  pub_marker_menu(feedback->marker_name,13);
}

void InteractiveMarkerInterface::changeForceModeCb( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback ){
  ROS_INFO("%s changeForceMode",feedback->marker_name.c_str());
  changeMarkerForceMode(feedback->marker_name,0);
  pub_marker_menu(feedback->marker_name,12);

}
void InteractiveMarkerInterface::changeForceModeCb1( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback ){
  ROS_INFO("%s changeForceMode1",feedback->marker_name.c_str());
  changeMarkerForceMode(feedback->marker_name,1);
  pub_marker_menu(feedback->marker_name,12);

}
void InteractiveMarkerInterface::changeForceModeCb2( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback ){
  ROS_INFO("%s changeForceMode2",feedback->marker_name.c_str());
  changeMarkerForceMode(feedback->marker_name,2);
  pub_marker_menu(feedback->marker_name,12);
}

void InteractiveMarkerInterface::targetPointMenuCB( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback ){
  ROS_INFO("targetPointMenu callback");
  pub_marker_menu(feedback->marker_name,41);
  control_state_.head_on_ ^= true;
  control_state_.init_head_goal_ = true;

  if(control_state_.head_on_){
    menu_head_.setCheckState(head_target_handle_, interactive_markers::MenuHandler::CHECKED);
  }else{
    menu_head_.setCheckState(head_target_handle_, interactive_markers::MenuHandler::UNCHECKED);
  }
  menu_head_.reApply(*server_);

  initControlMarkers();
}

void InteractiveMarkerInterface::ConstraintCb( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback )
{
  menu_handler.setCheckState( h_mode_last2, interactive_markers::MenuHandler::UNCHECKED );
  h_mode_last2 = feedback->menu_entry_id;
  menu_handler.setCheckState( h_mode_last2, interactive_markers::MenuHandler::CHECKED );

  switch(h_mode_last2-h_mode_constrained){
  case 0:
    pub_marker_menu(feedback->marker_name,23);
    ROS_INFO("send 23");
    break;
  case 1:
    pub_marker_menu(feedback->marker_name,24);
    ROS_INFO("send 24");
    break;
  default:
    ROS_INFO("Switching Arm Error");
    break;
  }

  menu_handler.reApply( *server_ );
  server_->applyChanges();

}

void InteractiveMarkerInterface::modeCb( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback )
{
  menu_handler.setCheckState( h_mode_last, interactive_markers::MenuHandler::UNCHECKED );
  h_mode_last = feedback->menu_entry_id;
  menu_handler.setCheckState( h_mode_last, interactive_markers::MenuHandler::CHECKED );

  switch(h_mode_last-h_mode_rightarm){
  case 0:
    pub_marker_menu(feedback->marker_name,20);
    ROS_INFO("send 20");
    break;
  case 1:
    pub_marker_menu(feedback->marker_name,21);
    ROS_INFO("send 21");
    break;
  case 2:
    pub_marker_menu(feedback->marker_name,22);
    ROS_INFO("send 22");
    break;
  default:
    ROS_INFO("Switching Arm Error");
    break;
  }

  menu_handler.reApply( *server_ );
  server_->applyChanges();
}

void InteractiveMarkerInterface::ikmodeCb( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback )
{
  menu_handler.setCheckState( h_mode_last3, interactive_markers::MenuHandler::UNCHECKED );
  h_mode_last3 = feedback->menu_entry_id;
  menu_handler.setCheckState( h_mode_last3, interactive_markers::MenuHandler::CHECKED );

  switch(h_mode_last3-h_mode_ikmode){
  case 0:
    pub_marker_menu(feedback->marker_name,36);
    ROS_INFO("send 36");
    break;
  case 1:
    pub_marker_menu(feedback->marker_name,37);
    ROS_INFO("send 37");
    break;
  default:
    ROS_INFO("Switching IKMode Error");
    break;
  }

  menu_handler.reApply( *server_ );
  server_->applyChanges();
}

void InteractiveMarkerInterface::updateHeadGoal( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback)
{
  ros::Time now = ros::Time(0);

  switch ( feedback->event_type )
  {
    case visualization_msgs::InteractiveMarkerFeedback::BUTTON_CLICK:
      ROS_INFO_STREAM( feedback->marker_name << " was clicked on." );
      break;
    case visualization_msgs::InteractiveMarkerFeedback::MENU_SELECT:
      ROS_INFO_STREAM(    "Marker " << feedback->marker_name
                       << " control " << feedback->control_name
                       << " menu_entry_id " << feedback->menu_entry_id);
      break;
    case visualization_msgs::InteractiveMarkerFeedback::POSE_UPDATE:
      proc_feedback(feedback);

      /*      geometry_msgs::PointStamped ps;
      ps.point = feedback-> pose.position;
      ps.header.frame_id = feedback->header.frame_id;
      ps.header.stamp = now;
      head_goal_pose_.pose = feedback->pose;
      head_goal_pose_.header = feedback->header;
      //mechanism_.pointHeadAction(ps, head_pointing_frame_, false);
      */
      break;
  }
}

//im_mode
//0:normal move  1:operationModel 2:operationalModelFirst
void InteractiveMarkerInterface::changeMarkerForceMode( std::string mk_name , int im_mode){
  ROS_INFO("changeMarkerForceMode  marker:%s  mode:%d\n",mk_name.c_str(),im_mode);
  interactive_markers::MenuHandler reset_handler;
  menu_handler_force = reset_handler;
  menu_handler_force1 = reset_handler;
  menu_handler_force2 = reset_handler;

  geometry_msgs::PoseStamped pose;
  pose.header.frame_id = base_frame;
  if ( target_frame != "" ) {
    tf::StampedTransform stf;
    geometry_msgs::TransformStamped mtf;
    tfl_.lookupTransform(target_frame, base_frame,
			 ros::Time(0), stf);
    tf::transformStampedTFToMsg(stf, mtf);
    pose.pose.position.x = mtf.transform.translation.x;
    pose.pose.position.y = mtf.transform.translation.y;
    pose.pose.position.z = mtf.transform.translation.z;
    pose.pose.orientation = mtf.transform.rotation;
    pose.header = mtf.header;
  }
  visualization_msgs::InteractiveMarker mk;
  //    mk.name = marker_name.c_str();
  mk.name = mk_name.c_str();
  mk.scale = 0.5;
  mk.header = pose.header;
  mk.pose = pose.pose;

  // visualization_msgs::InteractiveMarker mk =
  //   im_helpers::make6DofMarker(marker_name.c_str(), pose, 0.5,
  //                              true, false );
  visualization_msgs::InteractiveMarkerControl control;
    
  if ( false )
    {
      //int_marker.name += "_fixed";
      //int_marker.description += "\n(fixed orientation)";
      control.orientation_mode = visualization_msgs::InteractiveMarkerControl::FIXED;
    }

  control.orientation.w = 1;
  control.orientation.x = 1;
  control.orientation.y = 0;
  control.orientation.z = 0;
  control.name = "rotate_x";
  control.interaction_mode = visualization_msgs::InteractiveMarkerControl::ROTATE_AXIS;
  mk.controls.push_back(control);
  control.name = "move_x";
  control.interaction_mode = visualization_msgs::InteractiveMarkerControl::MOVE_AXIS;
  mk.controls.push_back(control);

  control.orientation.w = 1;
  control.orientation.x = 0;
  control.orientation.y = 1;
  control.orientation.z = 0;
  control.name = "rotate_z";
  control.interaction_mode = visualization_msgs::InteractiveMarkerControl::ROTATE_AXIS;
  mk.controls.push_back(control);
  // control.name = "move_z";
  // control.interaction_mode = visualization_msgs::InteractiveMarkerControl::MOVE_AXIS;
  // mk.controls.push_back(control);

  control.orientation.w = 1;
  control.orientation.x = 0;
  control.orientation.y = 0;
  control.orientation.z = 1;
  control.name = "rotate_y";
  control.interaction_mode = visualization_msgs::InteractiveMarkerControl::ROTATE_AXIS;
  mk.controls.push_back(control);
  // control.name = "move_y";
  // control.interaction_mode = visualization_msgs::InteractiveMarkerControl::MOVE_AXIS;
  // mk.controls.push_back(control);

  //add furuta
  switch(im_mode){
  case 0:
    menu_handler_force.insert("MoveMode",boost::bind( &InteractiveMarkerInterface::changeMoveModeCb, this, _1));
    menu_handler_force.insert("DeleteForce",boost::bind( &InteractiveMarkerInterface::noForceCb, this, _1));
    server_->insert( mk );
    server_->setCallback( mk.name,
			  boost::bind( &InteractiveMarkerInterface::proc_feedback, this, _1) );
    menu_handler_force.apply(*server_,mk.name);
      
    server_->applyChanges();
    break;
  case 1:
    mk.scale = 0.5;
    menu_handler_force1.insert("MoveMode",boost::bind( &InteractiveMarkerInterface::changeMoveModeCb1, this, _1));
    menu_handler_force1.insert("DeleteForce",boost::bind( &InteractiveMarkerInterface::noForceCb, this, _1));
    server_->insert( mk );
    server_->setCallback( mk.name,
			  boost::bind( &InteractiveMarkerInterface::proc_feedback, this, _1) );
    menu_handler_force1.apply(*server_,mk.name);
    
    server_->applyChanges();
    break;
  case 2:
    mk.scale = 0.5;
    menu_handler_force2.insert("MoveMode",boost::bind( &InteractiveMarkerInterface::changeMoveModeCb2, this, _1));
    menu_handler_force2.insert("DeleteForce",boost::bind( &InteractiveMarkerInterface::noForceCb, this, _1));
    server_->insert( mk );
    server_->setCallback( mk.name,
			  boost::bind( &InteractiveMarkerInterface::proc_feedback, this, _1) );
    menu_handler_force2.apply(*server_,mk.name);
 
    server_->applyChanges();
    break;
  default:
    break;
  }
  //menu_handler_force.insert("ResetForce",boost::bind( &InteractiveMarkerInterface::resetForceCb, this, _1));

  std::list<visualization_msgs::InteractiveMarker>::iterator it = imlist.begin();

  while( it != imlist.end() )  // listの末尾まで
    {
      if(it->name == mk_name.c_str()){
	imlist.erase(it);
	break;
      }
      it++;
    }
  imlist.push_back( mk );

  /*
    it = imlist.begin();
    while( it != imlist.end() )  // listの末尾まで
    {
    server_->insert( *it );
	
    server_->setCallback( it->name,
    boost::bind( &InteractiveMarkerInterface::proc_feedback, this, _1) );
	
    menu_handler_force.apply(*server_,it->name);
    it++;
    }*/
  ROS_INFO("add mk");
  /* add mk */
    

}

void InteractiveMarkerInterface::initBodyMarkers(void){
  geometry_msgs::PoseStamped ps;
  ps.header.stamp = ros::Time(0);

  double scale_factor = 1.02;

  //for head
  ps.header.frame_id = head_link_frame_;
  server_->insert(im_helpers::makeMeshMarker(head_link_frame_, head_mesh_,
					      ps, scale_factor));
  menu_head_.apply(*server_, head_link_frame_);

  //for right hand
  for(int i=0; i<rhand_mesh_.size(); i++){
    ps.header.frame_id = rhand_mesh_[i].link_name;
    ps.pose.orientation = rhand_mesh_[i].orientation;
    server_->insert(im_helpers::makeMeshMarker( rhand_mesh_[i].link_name, rhand_mesh_[i].mesh_file, ps, scale_factor));
    //,
    //boost::bind( &PR2MarkerControl::gripperButtonCB, this, _1, "toggle"), 
    //visualization_msgs::InteractiveMarkerFeedback::BUTTON_CLICK);
    //menu_head_.apply(*server_, rhand_mesh_[i].link_name);
  }


  /*
  ps.header.frame_id = "/r_gripper_palm_link";
  if (r_gripper_type_ == "pr2") ps.pose.position.x = mesh_offset;
  else ps.pose.position.x = 0.0;
  ps.pose.orientation = q_identity;
  server_.insert(makeMeshMarker( "r_gripper_palm_link", r_gripper_palm_path, ps, scale_factor),
		 boost::bind( &PR2MarkerControl::gripperButtonCB, this, _1, "toggle"), 
		 visualization_msgs::InteractiveMarkerFeedback::BUTTON_CLICK);
  menu_gripper_close_.apply(server_, "r_gripper_palm_link");
  
  ps.header.frame_id = "/r_gripper_l_finger_link";
  ps.pose.position.x = mesh_offset/2;
  ps.pose.orientation = q_identity;
  server_.insert(makeMeshMarker( "r_gripper_l_finger_link", r_gripper_finger_path, ps, scale_factor),
		 boost::bind( &PR2MarkerControl::gripperButtonCB, this, _1, "toggle"), 
		 visualization_msgs::InteractiveMarkerFeedback::BUTTON_CLICK);
  menu_gripper_close_.apply(server_, "r_gripper_l_finger_link");
  
  ps.header.frame_id = "/r_gripper_l_finger_tip_link";
  ps.pose.position.x = mesh_offset/4;
  ps.pose.orientation = q_identity;
  server_.insert(makeMeshMarker( "r_gripper_l_finger_tip_link", r_gripper_fingertip_path, ps, scale_factor*1.02),
		 boost::bind( &PR2MarkerControl::gripperButtonCB, this, _1, "toggle"), 
		 visualization_msgs::InteractiveMarkerFeedback::BUTTON_CLICK);
  menu_gripper_close_.apply(server_, "r_gripper_l_finger_tip_link");
  
  ps.header.frame_id = "/r_gripper_r_finger_link";
  ps.pose.position.x = mesh_offset/2;
  if (r_gripper_type_ == "pr2") ps.pose.orientation = q_rotateX180;
  else ps.pose.orientation = q_identity;
  server_.insert(makeMeshMarker( "r_gripper_r_finger_link", r_gripper_finger_path, ps, scale_factor),
		 boost::bind( &PR2MarkerControl::gripperButtonCB, this, _1, "toggle"), 
		 visualization_msgs::InteractiveMarkerFeedback::BUTTON_CLICK);
  menu_gripper_close_.apply(server_, "r_gripper_r_finger_link");
  
  ps.header.frame_id = "/r_gripper_r_finger_tip_link";
  ps.pose.position.x = mesh_offset/4;
  if (r_gripper_type_ == "pr2") ps.pose.orientation = q_rotateX180;
  else ps.pose.orientation = q_identity;
  server_.insert(makeMeshMarker( "r_gripper_r_finger_tip_link", r_gripper_fingertip_path, ps, scale_factor*1.02),
		 boost::bind( &PR2MarkerControl::gripperButtonCB, this, _1, "toggle"), 
		 visualization_msgs::InteractiveMarkerFeedback::BUTTON_CLICK);
  menu_gripper_close_.apply(server_, "r_gripper_r_finger_tip_link");
  
  // Try to clean up
  ps.pose.orientation = q_identity;
  */

}


void InteractiveMarkerInterface::initControlMarkers(void){

  if(control_state_.head_on_ && control_state_.init_head_goal_){
    control_state_.init_head_goal_ = false;
    head_goal_pose_.header.stamp = ros::Time(0);
    server_->insert(im_helpers::makeHeadGoalMarker( "head_point_goal", head_goal_pose_, 0.1),
		    boost::bind( &InteractiveMarkerInterface::updateHeadGoal, this, _1));
  }
  if(!control_state_.head_on_){
    server_->erase("head_point_goal");
  }
  server_->applyChanges();
}


void InteractiveMarkerInterface::initHandler(void){
  use_arm=2;

  bool use_menu;
  pnh_.param("force_mode_menu", use_menu, false );
  if(use_menu){
    menu_handler.insert("ForceMode",boost::bind( &InteractiveMarkerInterface::changeForceModeCb, this, _1));
  }
    
  pnh_.param("move_menu", use_menu, false );
  if(use_menu){
    menu_handler.insert("Move",boost::bind( &InteractiveMarkerInterface::pub_marker_menuCb, this, _1, jsk_interactive_marker::MarkerMenu::MOVE));
  }
    
  pnh_.param("force_move_menu", use_menu, false );
  if(use_menu){
    menu_handler.insert("ForceMove", boost::bind( &InteractiveMarkerInterface::pub_marker_menuCb, this, _1, jsk_interactive_marker::MarkerMenu::FORCE_MOVE));
  }

  pnh_.param("pick_menu", use_menu, false );
  if(use_menu){
    menu_handler.insert("Pick", boost::bind( &InteractiveMarkerInterface::pub_marker_menuCb, this, _1, jsk_interactive_marker::MarkerMenu::PICK));
  }

  pnh_.param("grasp_menu", use_menu, false );
  if(use_menu){
    menu_handler.insert("Grasp", boost::bind( &InteractiveMarkerInterface::pub_marker_menuCb, this, _1, jsk_interactive_marker::MarkerMenu::START_GRASP));
  }

  pnh_.param("stop_grasp_menu", use_menu, false );
  if(use_menu){
    menu_handler.insert("StopGrasp", boost::bind( &InteractiveMarkerInterface::pub_marker_menuCb, this, _1, jsk_interactive_marker::MarkerMenu::STOP_GRASP));
  }

  pnh_.param("set_origin_menu", use_menu, false );
  if(use_menu){
    menu_handler.insert("SetOrigin", boost::bind( &InteractiveMarkerInterface::pub_marker_menuCb, this, _1, jsk_interactive_marker::MarkerMenu::SET_ORIGIN));
  }

  pnh_.param("set_origin_to_rhand_menu", use_menu, false );
  if(use_menu){
    menu_handler.insert("SetOriginToRHand", boost::bind( &InteractiveMarkerInterface::pub_marker_menuCb, this, _1, jsk_interactive_marker::MarkerMenu::SET_ORIGIN_RHAND));
  }

  pnh_.param("set_origin_to_lhand_menu", use_menu, false );
  if(use_menu){
    menu_handler.insert("SetOriginToLHand", boost::bind( &InteractiveMarkerInterface::pub_marker_menuCb, this, _1, jsk_interactive_marker::MarkerMenu::SET_ORIGIN_LHAND));
  }

  pnh_.param("reset_marker_pos_menu", use_menu, false );
  if(use_menu){
    menu_handler.insert("ResetMarkerPos", boost::bind( &InteractiveMarkerInterface::pub_marker_menuCb, this, _1, jsk_interactive_marker::MarkerMenu::RESET_COORDS));
  }

  pnh_.param("manipulation_mode_menu", use_menu, false );
  if(use_menu){
    menu_handler.insert("ManipulationMode", boost::bind( &InteractiveMarkerInterface::pub_marker_menuCb, this, _1, jsk_interactive_marker::MarkerMenu::MANIP_MODE));
  }

  //    menu_handler.insert("ResetForce",boost::bind( &InteractiveMarkerInterface::resetForceCb, this, _1));
    
  //menu_handler.insert("OperationModel",boost::bind( &InteractiveMarkerInterface::AutoMoveCb, this, _1));
    
  //    menu_handler.insert("StartTeaching",boost::bind( &InteractiveMarkerInterface::StartTeachingCb, this, _1));


  /*    sub_menu_handle2 = menu_handler.insert( "Constraint" );
    
	h_mode_last2 = menu_handler.insert( sub_menu_handle2, "constrained", boost::bind( &InteractiveMarkerInterface::ConstraintCb,this, _1 ));
	menu_handler.setCheckState( h_mode_last2, interactive_markers::MenuHandler::UNCHECKED );
	h_mode_constrained = h_mode_last2;
	h_mode_last2 = menu_handler.insert( sub_menu_handle2, "unconstrained", boost::bind( &InteractiveMarkerInterface::ConstraintCb,this, _1 ));
	menu_handler.setCheckState( h_mode_last2, interactive_markers::MenuHandler::CHECKED );
  */
    
  //    menu_handler.insert("StopTeaching",boost::bind( &InteractiveMarkerInterface::StopTeachingCb, this, _1));
  //menu_handler.setCheckState(menu_handler.insert("SetForce",boost::bind( &InteractiveMarkerInterface::enableCb, this, _1)),interactive_markers::MenuHandler::UNCHECKED);
    

  pnh_.param("select_arm_menu", use_menu, false );
  if(use_menu){
    sub_menu_handle = menu_handler.insert( "SelectArm" );
    h_mode_last = menu_handler.insert( sub_menu_handle, "RightArm", boost::bind( &InteractiveMarkerInterface::modeCb,this, _1 ));
    menu_handler.setCheckState( h_mode_last, interactive_markers::MenuHandler::CHECKED );
    h_mode_rightarm = h_mode_last;
    h_mode_last = menu_handler.insert( sub_menu_handle, "LeftArm", boost::bind( &InteractiveMarkerInterface::modeCb,this, _1 ));
    menu_handler.setCheckState( h_mode_last, interactive_markers::MenuHandler::UNCHECKED );
    h_mode_last = menu_handler.insert( sub_menu_handle, "BothArms", boost::bind( &InteractiveMarkerInterface::modeCb,this, _1 ));
    menu_handler.setCheckState( h_mode_last, interactive_markers::MenuHandler::UNCHECKED );
    h_mode_last = h_mode_rightarm;
  }

  pnh_.param("ik_mode_menu", use_menu, false );
  if(use_menu){
    sub_menu_handle_ik = menu_handler.insert( "IKmode" );

    h_mode_last3 = menu_handler.insert( sub_menu_handle_ik, "Rotate t", boost::bind( &InteractiveMarkerInterface::ikmodeCb,this, _1 ));
    menu_handler.setCheckState( h_mode_last3, interactive_markers::MenuHandler::CHECKED );
    h_mode_ikmode = h_mode_last3;
    h_mode_last3 = menu_handler.insert( sub_menu_handle_ik, "Rotate nil", boost::bind( &InteractiveMarkerInterface::ikmodeCb,this, _1 ));
    menu_handler.setCheckState( h_mode_last3, interactive_markers::MenuHandler::UNCHECKED );
    h_mode_last3 = h_mode_ikmode;
  }


  interactive_markers::MenuHandler::EntryHandle sub_menu_handle_im_size;
  sub_menu_handle_im_size = menu_handler.insert( "IMsize" );
  menu_handler.insert( sub_menu_handle_im_size, "Large", boost::bind( &InteractiveMarkerInterface::IMSizeLargeCb, this, _1));
  menu_handler.insert( sub_menu_handle_im_size, "Middle", boost::bind( &InteractiveMarkerInterface::IMSizeMiddleCb, this, _1));
  menu_handler.insert( sub_menu_handle_im_size, "Small", boost::bind( &InteractiveMarkerInterface::IMSizeSmallCb, this, _1));

  pnh_.param("publish_marker_menu", use_menu, false );
  if(use_menu){
    //menu_handler.insert("ManipulationMode", boost::bind( &InteractiveMarkerInterface::pub_marker_menuCb, this, _1, jsk_interactive_marker::MarkerMenu::MANIP_MODE));
    menu_handler.insert("PublishMarker",boost::bind( &InteractiveMarkerInterface::publishMarkerCb, this, _1));
  }



  //--------- menu_handler 1 ---------------
  menu_handler1.insert("ForceMode",boost::bind( &InteractiveMarkerInterface::changeForceModeCb1, this, _1));

  //--------- menu_handler 2 ---------------
  menu_handler2.insert("ForceMode",boost::bind( &InteractiveMarkerInterface::changeForceModeCb2, this, _1));
  menu_handler2.insert("Move",boost::bind( &InteractiveMarkerInterface::moveCb, this, _1));
  /*
    sub_menu_handle = menu_handler2.insert( "SelectArm" );
    
    h_mode_last = menu_handler2.insert( sub_menu_handle, "RightArm", boost::bind( &InteractiveMarkerInterface::modeCb,this, _1 ));
    if(use_arm==0){
    menu_handler2.setCheckState( h_mode_last, interactive_markers::MenuHandler::CHECKED );
    }else{
    menu_handler2.setCheckState( h_mode_last, interactive_markers::MenuHandler::UNCHECKED );
    }
    
    h_mode_rightarm = h_mode_last;
    h_mode_last = menu_handler2.insert( sub_menu_handle, "LeftArm", boost::bind( &InteractiveMarkerInterface::modeCb,this, _1 ));
    if(use_arm==1){
    menu_handler2.setCheckState( h_mode_last, interactive_markers::MenuHandler::CHECKED );
    }else{
    menu_handler2.setCheckState( h_mode_last, interactive_markers::MenuHandler::UNCHECKED );
    }

    h_mode_last = menu_handler2.insert( sub_menu_handle, "BothArms", boost::bind( &InteractiveMarkerInterface::modeCb,this, _1 ));
    if(use_arm==2){
    menu_handler2.setCheckState( h_mode_last, interactive_markers::MenuHandler::CHECKED );
    }else{
    menu_handler2.setCheckState( h_mode_last, interactive_markers::MenuHandler::UNCHECKED );
    }
  */


  /* porting from PR2 marker control */
  //menu_head_.insert("Take Snapshot", boost::bind( &InteractiveMarkerInterface::snapshotCB, this ) );

  head_target_handle_ = menu_head_.insert( "Target Point", boost::bind( &InteractiveMarkerInterface::targetPointMenuCB,
									this, _1 ) );
  menu_head_.setCheckState(head_target_handle_, interactive_markers::MenuHandler::UNCHECKED);
  /*
    projector_handle_ = menu_head_.insert("Projector", boost::bind( &InteractiveMarkerInterface::projectorMenuCB,
    this, _1 ) );
    menu_head_.setCheckState(projector_handle_, MenuHandler::UNCHECKED);
  */
    
  /*
    menu_head_.insert( "Move Head To Center", boost::bind( &InteractiveMarkerInterface::centerHeadCB,
    this ) );
  */

}

//im_mode
//0:normal move  1:operationModel 2:operationalModelFirst
void InteractiveMarkerInterface::changeMarkerMoveMode( std::string mk_name , int im_mode){
  switch(im_mode){
  case 0:
    changeMarkerMoveMode( mk_name, im_mode , 0.5);
    break;

  case 1:
  case 2:
    changeMarkerMoveMode( mk_name, im_mode , 0.3);
    break;
  default:
    changeMarkerMoveMode( mk_name, im_mode , 0.3);
    break;
  }
}

void InteractiveMarkerInterface::changeMarkerMoveMode( std::string mk_name , int im_mode, float mk_size){
  geometry_msgs::PoseStamped pose;
  pose.header.frame_id = base_frame;
  changeMarkerMoveMode( mk_name, im_mode , mk_size, pose);
}

void InteractiveMarkerInterface::changeMarkerMoveMode( std::string mk_name , int im_mode, float mk_size, geometry_msgs::PoseStamped dist_pose){
  ROS_INFO("changeMarkerMoveMode  marker:%s  mode:%d\n",mk_name.c_str(),im_mode);
  interactive_markers::MenuHandler reset_handler;
  //    menu_handler2 = reset_handler;
  // menu_handler1 = reset_handler;
  // menu_handler = reset_handler;
  geometry_msgs::PoseStamped pose;
  if ( target_frame != "" ) {
    tf::StampedTransform stf;
    geometry_msgs::TransformStamped mtf;
    tfl_.lookupTransform(target_frame, base_frame,
			 ros::Time(0), stf);
    tf::transformStampedTFToMsg(stf, mtf);
    pose.pose.position.x = mtf.transform.translation.x;
    pose.pose.position.y = mtf.transform.translation.y;
    pose.pose.position.z = mtf.transform.translation.z;
    pose.pose.orientation = mtf.transform.rotation;
    pose.header = mtf.header;
  }else{
    pose = dist_pose;
  }

  visualization_msgs::InteractiveMarker mk;
    
  //0:normal move  1:operationModel 2:operationalModelFirst

  switch(im_mode){
  case 0:
    mk = im_helpers::make6DofMarker(mk_name.c_str(), pose, mk_size,
				    true, false );
    //mk.description = mk_name.c_str();
      
    server_->insert( mk );
    server_->setCallback( mk.name,
			  boost::bind( &InteractiveMarkerInterface::proc_feedback, this, _1) );
    menu_handler.apply(*server_,mk.name);
    server_->applyChanges();
    break;
  case 1:
    mk = im_helpers::make6DofMarker(mk_name.c_str(), pose, mk_size,
				    true, false );
    mk.description = mk_name.c_str();
      
    server_->insert( mk );
    server_->setCallback( mk.name,
			  boost::bind( &InteractiveMarkerInterface::proc_feedback, this, _1) );
    menu_handler1.apply(*server_,mk.name);
    server_->applyChanges();
    break;
      
  case 2:
    mk = im_helpers::make6DofMarker(mk_name.c_str(), pose, mk_size,
				    true, false );
    mk.description = mk_name.c_str();
      
    server_->insert( mk );
    server_->setCallback( mk.name,
			  boost::bind( &InteractiveMarkerInterface::proc_feedback, this, _1) );
    menu_handler2.apply(*server_,mk.name);
    server_->applyChanges();
    break;
  default:
    mk = im_helpers::make6DofMarker(mk_name.c_str(), pose, mk_size,
				    true, false );
    mk.description = mk_name.c_str();
      
    server_->insert( mk );
    server_->setCallback( mk.name,
			  boost::bind( &InteractiveMarkerInterface::proc_feedback, this, _1) );
    server_->applyChanges();
    break;
  }

  std::list<visualization_msgs::InteractiveMarker>::iterator it = imlist.begin(); // イテレータ

  while( it != imlist.end() )  // listの末尾まで
    {
      if(it->name == mk_name.c_str()){
	imlist.erase(it);
	break;
      }
      it++;
    }
  imlist.push_back( mk );

    
}

void InteractiveMarkerInterface::changeMarkerOperationModelMode( std::string mk_name ){
  interactive_markers::MenuHandler reset_handler;
  menu_handler = reset_handler;
  geometry_msgs::PoseStamped pose;
  pose.header.frame_id = base_frame;
  if ( target_frame != "" ) {
    tf::StampedTransform stf;
    geometry_msgs::TransformStamped mtf;
    tfl_.lookupTransform(target_frame, base_frame,
			 ros::Time(0), stf);
    tf::transformStampedTFToMsg(stf, mtf);
    pose.pose.position.x = mtf.transform.translation.x;
    pose.pose.position.y = mtf.transform.translation.y;
    pose.pose.position.z = mtf.transform.translation.z;
    pose.pose.orientation = mtf.transform.rotation;
    pose.header = mtf.header;
  }

  visualization_msgs::InteractiveMarker mk =
    im_helpers::make6DofMarker(mk_name.c_str(), pose, 0.5,
			       true, false );
  mk.description = mk_name.c_str();
  menu_handler.insert("ForceMode",boost::bind( &InteractiveMarkerInterface::changeForceModeCb, this, _1));

  std::list<visualization_msgs::InteractiveMarker>::iterator it = imlist.begin(); // イテレータ

  while( it != imlist.end() )  // listの末尾まで
    {
      if(it->name == mk_name.c_str()){
	imlist.erase(it);
	break;
      }
      it++;
    }
  imlist.push_back( mk );
  server_->insert( mk );
     
  server_->setCallback( mk.name,
			boost::bind( &InteractiveMarkerInterface::proc_feedback, this, _1) );
     
  menu_handler.apply(*server_,mk.name);
  server_->applyChanges();
}


InteractiveMarkerInterface::InteractiveMarkerInterface () : nh_(), pnh_("~"), tfl_(nh_) {
  pnh_.param("marker_name", marker_name, std::string ( "100") );
  pnh_.param("server_name", server_name, std::string ("") );
  pnh_.param("base_frame", base_frame, std::string ("/base_link") );
  pnh_.param("target_frame", target_frame, std::string ("") );
  //pnh_.param("fix_marker", fix_marker, true);

  if ( server_name == "" ) {
    server_name = ros::this_node::getName();
  }

  pub_ =  pnh_.advertise<jsk_interactive_marker::MarkerPose> ("pose", 1);
  pub_move_ =  pnh_.advertise<jsk_interactive_marker::MarkerMenu> ("move_flag", 1);

  serv_set_ = pnh_.advertiseService("set_pose",
				    &InteractiveMarkerInterface::set_cb, this);
  serv_markers_set_ = pnh_.advertiseService("set_markers",
					    &InteractiveMarkerInterface::markers_set_cb, this);
  serv_markers_del_ = pnh_.advertiseService("del_markers",
					    &InteractiveMarkerInterface::markers_del_cb, this);
  serv_reset_ = pnh_.advertiseService("reset_pose",
				      &InteractiveMarkerInterface::reset_cb, this);

  server_.reset( new interactive_markers::InteractiveMarkerServer(server_name, "sid", false) );

  pnh_.param<std::string>("head_link_frame", head_link_frame_, "head_tilt_link");
  pnh_.param<std::string>("head_mesh", head_mesh_, "package://pr2_description/meshes/head_v0/head_tilt.dae");
  
  XmlRpc::XmlRpcValue v;
  pnh_.param("mesh_config", v, v);
  loadMeshes(v);

  head_goal_pose_.pose.position.x = 1.0;
  head_goal_pose_.pose.position.z = 1.0;
  head_goal_pose_.header.frame_id = base_frame;

  initHandler();
  initBodyMarkers();
  initControlMarkers();
  changeMarkerMoveMode(marker_name.c_str(),0);
}

void InteractiveMarkerInterface::loadMeshes(XmlRpc::XmlRpcValue val){
  for(int i=0; i< val["r_hand"].size(); i++){
    XmlRpc::XmlRpcValue nval = val["r_hand"][i];
    MeshProperty m;
    m.link_name = (std::string)nval["link"];
    m.mesh_file = (std::string)nval["mesh"];

    if(nval.hasMember("orient")){
      XmlRpc::XmlRpcValue orient = nval["orient"];
      std::cerr << "load_link: " << orient["x"] << std::endl;
      m.orientation.x = (double)orient["x"];
      m.orientation.y = (double)orient["y"];
      m.orientation.z = (double)orient["z"];
      m.orientation.w = (double)orient["w"];
    }
    rhand_mesh_.push_back(m);
    std::cerr << "load_link: " << nval["link"] << std::endl;
  }
  
  /*
  for(int i =0; i < v.size(); i++)
    {
      //node_names_.push_back(v[i]);
      std::cerr << "node_names: " << v[i] << std::endl;
    }
  */
    
}

bool InteractiveMarkerInterface::markers_set_cb ( jsk_interactive_marker::MarkerSetPose::Request &req,
						  jsk_interactive_marker::MarkerSetPose::Response &res ) {
  bool setalready = false;

  std::list<visualization_msgs::InteractiveMarker>::iterator it = imlist.begin();
  while( it != imlist.end() )  // listの末尾まで
    {
      if( it->name == req.marker_name){
	setalready = true;
	break;
      }
      it++;
    }

  if(setalready){
    server_->setPose(req.marker_name, req.pose.pose, req.pose.header);
    server_->applyChanges();
    return true;
  }else{
    /*
    if(req.marker_name==0){
      changeMarkerMoveMode(name,2);
    }else{
      changeMarkerMoveMode(name,1);
    }
    server_->setPose(name, req.pose.pose, req.pose.header);
    //    menu_handler.apply(*server_,mk.name)Z
    server_->applyChanges();
    return true;
    */
    return true;
  }
}
  
bool InteractiveMarkerInterface::markers_del_cb ( jsk_interactive_marker::MarkerSetPose::Request &req,
						  jsk_interactive_marker::MarkerSetPose::Response &res ) {
  
  server_->erase(req.marker_name);
  server_->applyChanges();
  std::list<visualization_msgs::InteractiveMarker>::iterator it = imlist.begin();
  while( it != imlist.end() )  // listの末尾まで
    {
      if( it->name == req.marker_name){
	imlist.erase(it);
	break;
      }
      it++;
    }
    
  return true;
    
}

bool InteractiveMarkerInterface::set_cb ( jsk_interactive_marker::SetPose::Request &req,
					  jsk_interactive_marker::SetPose::Response &res ) {
  if ( req.markers.size() > 0 ) {
    visualization_msgs::InteractiveMarker mk;
    if ( server_->get(marker_name, mk) ) {
      visualization_msgs::InteractiveMarkerControl mkc;
      mkc.name = "additional_marker";
      mkc.always_visible = true;
      mkc.markers = req.markers;
      // delete added marker
      for ( std::vector<visualization_msgs::InteractiveMarkerControl>::iterator it
	      =  mk.controls.begin();
	    it != mk.controls.end(); it++ ) {
	if ( it->name == mkc.name ){
	  mk.controls.erase( it );
	  break;
	}
      }
      mk.controls.push_back( mkc );
    }
  }

  server_->setPose(marker_name, req.pose.pose, req.pose.header);
  server_->applyChanges();
  return true;
}

bool InteractiveMarkerInterface::reset_cb ( jsk_interactive_marker::SetPose::Request &req,
					    jsk_interactive_marker::SetPose::Response &res ) {
  geometry_msgs::PoseStamped pose;
  pose.header.frame_id = base_frame;
  if ( target_frame != "" ) {
    tf::StampedTransform stf;
    geometry_msgs::TransformStamped mtf;
    tfl_.lookupTransform(base_frame, target_frame,
			 ros::Time(0), stf);
    tf::transformStampedTFToMsg(stf, mtf);
    pose.pose.position.x = mtf.transform.translation.x;
    pose.pose.position.y = mtf.transform.translation.y;
    pose.pose.position.z = mtf.transform.translation.z;
    pose.pose.orientation = mtf.transform.rotation;
    // pose.header = mtf.header;
    // pose.header.stamp = ros::Time::Now();
    // pose.header.frame_id = target_frame;
    server_->setPose(marker_name, pose.pose, pose.header);
  } else {
    server_->setPose(marker_name, pose.pose);
  }
  server_->applyChanges();
  return true;
}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "jsk_marker_interface");
  InteractiveMarkerInterface imi;
  ros::spin();

  return 0;
}