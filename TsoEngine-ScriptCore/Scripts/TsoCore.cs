using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace Tso
{
    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void NativeLOG(string msg);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetTranslation(ulong uuid , out Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetTranslation(ulong uuid, Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetSpriteIndex(ulong uuid , Vector3 spriteIndex);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetRotationZ(ulong uuid, out float rotationZ);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetRotationZ(ulong uuid, float rotationZ);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool IsKeyPressed(Keycode keycode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Fire(ulong uuid);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void DestroyEntity(ulong uuid);
    }
}
