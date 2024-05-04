using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tso
{
    public class Entity
    {

        public Entity() { }

        public Entity(ulong uuid){ UUID = uuid; Console.WriteLine("ur father has made uuid done"); }

        public void Test()
        {
            Console.WriteLine("hello world from Entity class");
        }

        public virtual void OnCreate() { }

        public virtual void OnUpdate(float ts) { }

        public virtual void OnCollide(ulong uuid) {
            Console.WriteLine("collide by uuid " + uuid); ;

        }

        protected ulong UUID { get; set; }
    }
}
