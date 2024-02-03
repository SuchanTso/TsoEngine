using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tso
{
    public class Player : Entity
    {
        public Player() { }

        public override void  OnCreate()
        {
            InternalCalls.NativeLOG("Tso log from c#!");
        }

        public override void OnUpdate(float ts)
        {
            
        }
    }
}
