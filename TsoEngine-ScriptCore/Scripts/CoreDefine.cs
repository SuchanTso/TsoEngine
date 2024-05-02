using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tso
{
    public struct Vector3
    {
        public float x, y, z;
    };

    public struct Vector2
    {
        public float X, Y;
    };

    public enum AnimationState
    {
        Idle = 0,
        Walk = 1,
        Climb = 2,
    }
}
