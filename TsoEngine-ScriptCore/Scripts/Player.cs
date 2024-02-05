using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tso
{
    public class Player : Entity
    {
        public Player()
        {
            InternalCalls.NativeLOG("None param constructor");
        }

        public Player(ulong uuid):base(uuid)
        {
            UUID = uuid;
            InternalCalls.NativeLOG("uuid constructor");
        }

        public override void  OnCreate()
        {
            InternalCalls.NativeLOG("Tso log from c# ");
            
            Vector3 translation = new Vector3();
            InternalCalls.GetTranslation(UUID, out translation);
            Console.WriteLine($"translation = {translation.x} , {translation.y} , {translation.z}");
        }

        public override void OnUpdate(float ts)
        {
            
        }
    }
}
