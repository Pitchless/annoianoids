
#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"

/*
 TODO: stuff mess.
 Maybe the way out is to define stuff as an interface, then anything
 that wants to be stuff can mix that in.
 
 We do want all the attraction point stuff.
 */

/*
 * The world is made of Stuff, the leaves on the tree. Virtual base
 * (interface) classes need to impliment to be added to Things.
 */
class Stuff {
protected:
private:
public:
  virtual ~Stuff(){};
  virtual void update() = 0;
  virtual void draw() = 0;
};
typedef shared_ptr<Stuff> StuffPtr;
typedef vector<StuffPtr> StuffVec;

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

    size_t size() {
        return stuff_.size();
    };

    void add(StuffPtr stuff) {
        stuff_.push_back(stuff);
    };

    void clear() {
        //for (int i=0; i<stuff_.size(); i++) {
        //  stuff_[i]->destroy();
        //}
        stuff_.clear();
    };

    void update() {
        for (int i=0; i<stuff_.size(); i++) {
            stuff_[i]->update();
        }
    };

    void draw() {
        for (int i=0; i<stuff_.size(); i++) {
            stuff_[i]->draw();
        }
    };
};
