using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tso
{
    public class Entity
    {
        public void Test()
        {
            Console.WriteLine("hello world from Entity class");
        }

        public virtual void OnCreate() { }

        public virtual void OnUpdate(float ts) { }

        internal ulong UUID { get; set; }
    }
}
