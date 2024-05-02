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
            InternalCalls.GetTranslation(UUID, out m_Tranlation);
        }

        public override void OnUpdate(float ts)
        {
            m_Move = false;
            if (InternalCalls.IsKeyPressed(Keycode.TSO_KEY_D))
            {
                m_Tranlation.x += m_Vecloty * ts;
                m_Idle.SetWalk();
                m_Move = true;
            }
            else if (InternalCalls.IsKeyPressed(Keycode.TSO_KEY_A))
            {
                m_Tranlation.x -= m_Vecloty * ts;
                m_Idle.SetWalk();
                m_Move = true;

            }

            if (InternalCalls.IsKeyPressed(Keycode.TSO_KEY_W))
            {
                m_Tranlation.y += m_Vecloty * ts;
                m_Idle.SetClimb();
                m_Move = true;

            }
            else if (InternalCalls.IsKeyPressed(Keycode.TSO_KEY_S))
            {
                m_Tranlation.y -= m_Vecloty * ts;
                m_Idle.SetClimb();
                m_Move = true;

            }

            if (!m_Move)
            {
                m_Idle.SetIdle();
            }
            m_Idle.OnUpdate(ts, UUID);
            InternalCalls.SetTranslation(UUID, m_Tranlation);

        }

        internal Vector3 m_Tranlation = new Vector3();
        internal float m_Vecloty = 1.5f;
        internal bool m_Move = false;
        internal SpriteAnimationIdle m_Idle = new SpriteAnimationIdle();
    }
}
