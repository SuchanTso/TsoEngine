using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tso
{
    
    public class SpriteAnimationIdle
    {
        public static Vector2[] SpriteFrameIdle  = new Vector2[] { new Vector2 { X = 26F, Y = 28F }, new Vector2 { X = 27F, Y = 28F } };
        public static Vector2[] SpriteFrameWalk  = new Vector2[] { new Vector2 { X = 28F, Y = 28F }, new Vector2 { X = 29F, Y = 28F } };
        public static Vector2[] SpriteFrameClimb = new Vector2[] { new Vector2 { X = 24F, Y = 28F }, new Vector2 { X = 25F, Y = 28F } };


        public SpriteAnimationIdle() {
            m_Time = 0;
            m_Frame.x = m_SpriteFrame[0].X;
            m_Frame.y = m_SpriteFrame[0].Y;
            m_FrameDuration = m_Duration / m_SpriteFrame.Length;
            m_SpriteFrame = SpriteFrameIdle;
        }

        

        public void OnUpdate(float ts , ulong uuid)
        {
            if(m_FrameDuration <= 0 || uuid == 0) {  return; }
            m_Time += ts;
            if (m_Time >= m_Duration)
            {
                m_Time -= m_Duration;
            }
            float fframeIndex = m_Time / m_FrameDuration;
            int frameIndex = (int)fframeIndex;
            m_Frame.x = m_SpriteFrame[frameIndex].X;
            m_Frame.y = m_SpriteFrame[frameIndex].Y;
            InternalCalls.SetSpriteIndex(uuid, m_Frame);
        }

        public void SetIdle()
        {
            if(m_LastState != m_State) {
                m_Time = 0;
                m_SpriteFrame = SpriteFrameIdle;
                m_LastState = m_State;
            }
            
            m_State = AnimationState.Idle;
            
        }

        public void SetWalk()
        {
            if (m_LastState != m_State) {
                m_SpriteFrame = SpriteFrameWalk;
                m_LastState = m_State;
                m_Time = 0;
            }
            m_State = AnimationState.Walk;
        }

        public void SetClimb()
        {
            if (m_LastState != m_State)
            {
                m_SpriteFrame = SpriteFrameClimb;
                m_LastState = m_State;
                m_Time = 0;
            }
            m_State = AnimationState.Climb;
        }


        internal Vector2[] m_SpriteFrame = new Vector2[2];
        internal float m_Duration = 0.5F;
        internal AnimationState m_State = AnimationState.Idle;
        internal AnimationState m_LastState = AnimationState.Idle;
        internal Vector3 m_Frame = new Vector3 ();
        internal float m_Time = 0f;
        internal Vector3 test = new Vector3 ();
        internal float m_FrameDuration = 0f;
    }
}
