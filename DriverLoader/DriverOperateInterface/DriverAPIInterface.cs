using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace DriverOperateInterface
{
    public class DriverAPIInterface
    {
        #region 常量
        public const long NDER_GETLASTERROR = -1;
        public const long NDER_SUCCESS = 0;
        public const long NDER_FILE_NO_FOUND = 1;
        public const long NDER_SERVICE_NOT_OPEN = 2;
        public const long NDER_SERVICE_NOT_STARTED = 3;
        public const long NDER_SERVICE_ALREADY_OPENED = 4;
        public const long NDER_SERVICE_ALREADY_STARTED = 5;
        public const long NDER_SERVICE_IO_PENDING = 6;

        public const long ERROR_FILE_NOT_FOUND = 2L;
        public const long ERROR_SERVICE_NOT_ACTIVE = 1062L;
        public const long ERROR_SERVICE_ALREADY_RUNNING = 1056L;

        public const long DRIVER_CLOSED = 0;
        public const long  DRIVER_OPENED = 1;
        public const long  DRIVER_STARTED = 2;
        public const long  DRIVER_PAUSED = 3;
        public const long  DRIVER_BUSY = 4;
        #endregion

        #region API接口
        public const string DRIVEROPERATEDLL = "DriverLibrary.dll";

        [DllImport(DRIVEROPERATEDLL, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern int add(int a, int b);

        [DllImport(DRIVEROPERATEDLL, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern IntPtr Create([MarshalAs(UnmanagedType.LPTStr)] string szDrvSvcName, [MarshalAs(UnmanagedType.LPTStr)]string szDrvDisplayName, [MarshalAs(UnmanagedType.LPTStr)] string filePath);

        [DllImport(DRIVEROPERATEDLL, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern IntPtr GetStatus([MarshalAs(UnmanagedType.LPTStr)]string driverServiceName);

        [DllImport(DRIVEROPERATEDLL, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern IntPtr Load();

        [DllImport(DRIVEROPERATEDLL, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern IntPtr Unload(int dwWaitMilliseconds);

        [DllImport(DRIVEROPERATEDLL, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern IntPtr Delete();

        [DllImport(DRIVEROPERATEDLL, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern IntPtr Open([MarshalAs(UnmanagedType.LPTStr)]string szDrvSvcName);

        [DllImport(DRIVEROPERATEDLL, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern IntPtr Close();

        [DllImport(DRIVEROPERATEDLL, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern void OperateDriver();
        #endregion

        #region 公共方法
        [DllImport(DRIVEROPERATEDLL, CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool IsRunasAdmin();
        #endregion
    }
}
