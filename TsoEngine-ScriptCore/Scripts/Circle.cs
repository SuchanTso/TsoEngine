using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tso
{
    public class Circle : Entity
    {
        public override void OnUpdate(float ts)
        {
            m_Time += ts;

            m_Tranlation.x = (float)Math.Cos((double)m_Time);

            m_Tranlation.y = (float)Math.Sin((double)m_Time);

            double rotation =  (double)m_Time / Math.PI * 180 - 90;
            InternalCalls.SetTranslation(UUID, m_Tranlation);
            InternalCalls.SetRotationZ(UUID, (float)rotation);


        }

        internal float m_Time = new float();
        internal Vector3 m_Tranlation = new Vector3();

    }
}
