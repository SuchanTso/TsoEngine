using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tso
{
    public class Bullet : Entity
    {
        public Bullet() { }

        public override void OnCollide(ulong uuid)
        {
            base.OnCollide(uuid);
            InternalCalls.DestroyEntity(UUID);
        }
    }
}
