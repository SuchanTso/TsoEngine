//
//  CollideListener.hpp
//  TsoEngine
//
//  Created by user on 2024/1/19.
//

#ifndef CollideListener_hpp
#define CollideListener_hpp
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"


#include <stdio.h>

namespace Tso{
class NativeContactListener : public b2ContactListener{
public:
    virtual void BeginContact(b2Contact* contact) override;
};
}

#endif /* CollideListener_hpp */
