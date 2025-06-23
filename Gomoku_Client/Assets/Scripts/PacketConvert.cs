using System;
using System.Runtime.InteropServices;

public static class PacketConvert
{
    // 구조체를 바이트 배열로 변환 (클라이언트 -> 서버)
    public static byte[] StructureToByteArray(object obj)
    {
        int size = Marshal.SizeOf(obj);
        byte[] arr = new byte[size];
        IntPtr ptr = Marshal.AllocHGlobal(size); // 관리되지 않는 메모리 할당

        Marshal.StructureToPtr(obj, ptr, true);
        Marshal.Copy(ptr, arr, 0, size);
        Marshal.FreeHGlobal(ptr); // 메모리 해제

        return arr;
    }

    // 바이트 배열을 구조체로 변환 (서버 -> 클라이언트)
    public static T ByteArrayToStructure<T>(byte[] bytearray) where T : struct
    {
        T st;
        int size = Marshal.SizeOf(typeof(T));
        if (size > bytearray.Length)
        {
            throw new Exception("Byte array is smaller than the structure size.");
        }

        IntPtr ptr = Marshal.AllocHGlobal(size);

        Marshal.Copy(bytearray, 0, ptr, size);
        st = (T)Marshal.PtrToStructure(ptr, typeof(T));
        Marshal.FreeHGlobal(ptr);

        return st;
    }
}