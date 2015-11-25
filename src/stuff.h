
#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "World.h"

/*
 * The world is made of Stuff, the leaves on the tree.
 */
typedef ofxBox2dBaseShape Stuff;
typedef shared_ptr<Stuff> StuffPtr;
typedef vector<StuffPtr> StuffVec;

/*
class Stuff : public ofxBox2dBaseShape {
protected:
private:
public:
  Stuff() {};
  virtual ~Stuff(){};
  virtual void create(b2World* world) {};
  virtual void update() {};
  virtual void draw() {};
};
*/

/*
 * Stuff is orgnaised into Things, the branches of the tree.
 */
class Thing;
typedef shared_ptr<Thing> ThingPtr;
typedef vector<ThingPtr> ThingVec;

class Thing {
protected:
  StuffVec stuff_;
public:
  Thing() {};

  //void create(World world) {
  //  for (int i=0; i<stuff.size(); i++) {
  //    stuff[i]->create(world);
  //  }
  //};
  
  size_t size() { return stuff_.size(); };
  
  void add(StuffPtr stuff) { stuff_.push_back(stuff); };

  void clear(){
    //for (int i=0; i<stuff_.size(); i++) {
    //  stuff_[i]->destroy();
    //}
    stuff_.clear();
  };
  
  void update(){
    for (int i=0; i<stuff_.size(); i++) {
      stuff_[i]->update();
    }
  };

  void draw(){
    for (int i=0; i<stuff_.size(); i++) {
      stuff_[i]->draw();
    }
  };
};