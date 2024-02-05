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

    }
}
