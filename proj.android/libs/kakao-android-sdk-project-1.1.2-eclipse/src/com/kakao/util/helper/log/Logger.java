package com.kakao.util.helper.log;

import android.util.Log;

//import com.kakao.util.BuildConfig;

import java.util.HashSet;
import java.util.Set;

public class Logger {
    public static enum DeployPhase {
        Local, Alpha, Sandbox, Beta, Release;

        public static DeployPhase convert(String i) {
            for (DeployPhase current : values()) {
                if (current.toString().toLowerCase().equals(i)) {
                    return current;
                }
            }
            return Release;
        }

        public static DeployPhase current() {
            return Release;
        }
    }

    private Logger() {
    }

//    private static LoggerConfig loggerConfig = Config.newKakaoTalkLoggerConfig();
    private static LoggerConfig loggerConfig = initLogConfig();

    private static final int LOG_SEGMENT_SIZE = 2000;

    private static LoggerConfig initLogConfig() {
        boolean showLogs = false;

        int printLogLevel = Log.DEBUG;
        if(!showLogs && DeployPhase.current() == DeployPhase.Release ) {
            printLogLevel = Log.WARN;
        }
        LoggerConfig.Builder builder = new LoggerConfig.Builder();
        builder = builder.setDefaultTag(Tag.DEFAULT).setStackPrefix("com.kakao.sdk").setPrintLoggerLevel(printLogLevel);

        Set<String> set = new HashSet<String>();
        set.add(Logger.class.getName());
        builder.setIgnoreSet(set);
        return builder.build();
    }

    private static int printLog(Tag tag, int logLevel, boolean withStack, String msg) {
        if (msg == null) {
            return 0;
        }
        String message = loggerConfig.getMessage(withStack, msg);
        if (!loggerConfig.isPrintLoggable(logLevel)) {
            return 0;
        }
        if (message == null) {
            message = loggerConfig.getMessage(withStack, msg);
        }
        if (message == null) {
            return 0;
        }

        String tagMsg = tag.tag();
        int messageLen = message.length();
        int curIdx = 0, depth = 0;

        int totalPrintLen = 0;
        while (curIdx < messageLen) {
            int remainLen = messageLen - curIdx;
            if (remainLen > LOG_SEGMENT_SIZE) {
                remainLen = LOG_SEGMENT_SIZE;
            }

            totalPrintLen += printLogPartially(logLevel, tagMsg, message.substring(curIdx, curIdx + remainLen), depth++);
            curIdx += remainLen;
        }
        return totalPrintLen;
    }

    private static int printLogPartially(int logLevel, String tagMsg, String msg, int depth) {
        int msgLen = msg.length();

        int writtenLen = 0;
        String prefix = "";
        if (depth > 0) {
            prefix = String.format("Cont(%d) ", depth);
        }

        final String printMsg;
        if (msgLen > LOG_SEGMENT_SIZE) {
            printMsg = msg.substring(0, LOG_SEGMENT_SIZE);
        } else {
            printMsg = msg;
        }

        switch (logLevel) {
            case LoggerConfig.DEV:
                writtenLen = Log.d(tagMsg, prefix + printMsg);
                break;

            case LoggerConfig.VERBOSE:
                writtenLen = Log.v(tagMsg, prefix + printMsg);
                break;

            case LoggerConfig.DEBUG:
                writtenLen = Log.d(tagMsg, prefix + printMsg);
                break;

            case LoggerConfig.INFO:
                writtenLen = Log.i(tagMsg, prefix + printMsg);
                break;

            case LoggerConfig.WARN:
                writtenLen = Log.w(tagMsg, prefix + printMsg);
                break;

            case LoggerConfig.ERROR:
                writtenLen = Log.e(tagMsg, prefix + printMsg);
                break;
        }

        return writtenLen;
    }

    public static String getCallerTraceInfo(@SuppressWarnings("rawtypes")
    Class klass) {
        if (!loggerConfig.isPrintLoggable(LoggerConfig.DEBUG)) {
            return "unknown caller";
        }
        StackTraceElement[] stacks = Thread.currentThread().getStackTrace();
        String threadName = Thread.currentThread().getName();
        StackTraceElement stack = null;
        String className = null;
        String callerClassName = klass.getName();
        boolean found = false;
        for (StackTraceElement stack1 : stacks) {
            className = stack1.getClassName();
            if (className.startsWith(callerClassName)) {
                found = true;
            } else if (found) {
                stack = stack1;
                break;
            }
        }

        if (stack == null) {
            return null;
        }

        String klassName = stack.getClassName();
        String method = stack.getMethodName();
        int line = stack.getLineNumber();
        return String.format("{%s}-[%s.%s():%d]", threadName, klassName, method, line);
    }

    public static String getCallerTraceInfo(Object caller) {
        return getCallerTraceInfo(caller.getClass());
    }

    public static String getStackTraceString(Throwable tr) {
        if (tr == null) return "";
        return Log.getStackTraceString(tr);
    }

    // verbose with tag
    public static int vt(Tag tag, String format, Object... args) {
        return vt(tag, true, format, args);
    }

    public static int vt(Tag tag, String msg) {
        return vt(tag, true, msg);
    }

    public static int vt(Tag tag, Throwable tr) {
        return vt(tag, getStackTraceString(tr));
    }

    public static int vt(Tag tag, String msg, Throwable tr) {
        return vt(tag, "%s\n%s", msg, getStackTraceString(tr));
    }

    private static int vt(Tag tag, boolean withStack, String format, Object... args) {
        if (loggerConfig.isPrintLoggable(LoggerConfig.VERBOSE)) {
            String message = null;
            if (args != null && args.length > 0) {
                message = String.format(format, args);
            } else {
                message = format;
            }
            return printLog(tag, LoggerConfig.VERBOSE, withStack, message);
        } else {
            return 0;
        }
    }

    public static int vt(Tag tag, boolean withStack, String msg) {
        return printLog(tag, LoggerConfig.VERBOSE, withStack, msg);
    }

    // verbose with default tag
    public static int v(String format, Object... args) {
        return vt(loggerConfig.getDefaultTag(), format, args);
    }

    public static int v(boolean withStack, String format, Object... args) {
        return vt(loggerConfig.getDefaultTag(), withStack, format, args);
    }

    public static int v(Throwable tr) {
        return vt(loggerConfig.getDefaultTag(), tr);
    }

    public static int v(String msg, Throwable tr) {
        return vt(loggerConfig.getDefaultTag(), msg, tr);
    }

    // debug with tag
    public static int dt(Tag tag, String format, Object... args) {
        return dt(tag, true, format, args);
    }

    public static int dt(Tag tag, String msg) {
        return dt(tag, true, msg);
    }

    public static int dt(Tag tag, Throwable tr) {
        return dt(tag, getStackTraceString(tr));
    }

    public static int dt(Tag tag, String msg, Throwable tr) {
        return dt(tag, "%s\n%s", msg, getStackTraceString(tr));
    }

    private static int dt(Tag tag, boolean withStack, String format, Object... args) {
        if (loggerConfig.isPrintLoggable(LoggerConfig.DEBUG)) {
            String message = null;
            if (args != null && args.length > 0) {
                message = String.format(format, args);
            } else {
                message = format;
            }
            return printLog(tag, LoggerConfig.DEBUG, withStack, message);
        } else {
            return 0;
        }
    }

    public static int dt(Tag tag, boolean withStack, String msg) {
        return printLog(tag, LoggerConfig.DEBUG, withStack, msg);
    }

    // debug with default tag
    public static int d(String format, Object... args) {
        return dt(loggerConfig.getDefaultTag(), format, args);
    }

    public static int d(boolean withStack, String format, Object... args) {
        return dt(loggerConfig.getDefaultTag(), withStack, format, args);
    }

    public static int d(String msg) {
        return dt(loggerConfig.getDefaultTag(), true, msg);
    }

    public static int d(Throwable tr) {
        return dt(loggerConfig.getDefaultTag(), tr);
    }

    public static int d(String msg, Throwable tr) {
        return dt(loggerConfig.getDefaultTag(), msg, tr);
    }

    // info with tag
    public static int it(Tag tag, String format, Object... args) {
        return it(tag, true, format, args);
    }

    public static int it(Tag tag, String msg) {
        return it(tag, true, msg);
    }

    public static int it(Tag tag, Throwable tr) {
        return it(tag, getStackTraceString(tr));
    }

    public static int it(Tag tag, String msg, Throwable tr) {
        return it(tag, "%s\n%s", msg, getStackTraceString(tr));
    }

    private static int it(Tag tag, boolean withStack, String format, Object... args) {
        if (loggerConfig.isPrintLoggable(LoggerConfig.INFO)) {
            String message = null;
            if (args != null && args.length > 0) {
                message = String.format(format, args);
            } else {
                message = format;
            }
            return printLog(tag, LoggerConfig.INFO, withStack, message);
        } else {
            return 0;
        }
    }

    public static int it(Tag tag, boolean withStack, String msg) {
        return printLog(tag, LoggerConfig.INFO, withStack, msg);
    }

    // info with default tag.
    public static int i(String format, Object... args) {
        return it(loggerConfig.getDefaultTag(), format, args);
    }

    public static int i(boolean withStack, String format, Object... args) {
        return it(loggerConfig.getDefaultTag(), withStack, format, args);
    }

    public static int i(String msg) {
        return it(loggerConfig.getDefaultTag(), true, msg);
    }

    public static int i(Throwable tr) {
        return it(loggerConfig.getDefaultTag(), tr);
    }

    public static int i(String msg, Throwable tr) {
        return it(loggerConfig.getDefaultTag(), msg, tr);
    }

    // warn with tag
    public static int wt(Tag tag, String format, Object... args) {
        return wt(tag, true, format, args);
    }

    public static int wt(Tag tag, String msg) {
        return wt(tag, true, msg);
    }

    public static int wt(Tag tag, Throwable tr) {
        return wt(tag, getStackTraceString(tr));
    }

    public static int wt(Tag tag, String msg, Throwable tr) {
        return wt(tag, "%s\n%s", msg, getStackTraceString(tr));
    }

    private static int wt(Tag tag, boolean withStack, String format, Object... args) {
        if (loggerConfig.isPrintLoggable(LoggerConfig.WARN)) {
            String message = String.format(format, args);
            return printLog(tag, LoggerConfig.WARN, withStack, message);
        } else {
            return 0;
        }
    }

    public static int wt(Tag tag, boolean withStack, String msg) {
        return printLog(tag, LoggerConfig.WARN, withStack, msg);
    }

    // warn with default tag
    public static int w(String format, Object... args) {
        return wt(loggerConfig.getDefaultTag(), format, args);
    }

    public static int w(boolean withStack, String format, Object... args) {
        return wt(loggerConfig.getDefaultTag(), withStack, format, args);
    }

    public static int w(String msg) {
        return wt(loggerConfig.getDefaultTag(), true, msg);
    }

    public static int w(Throwable tr) {
        return wt(loggerConfig.getDefaultTag(), tr);
    }

    public static int w(String msg, Throwable tr) {
        return wt(loggerConfig.getDefaultTag(), msg, tr);
    }

    // error with tag
    public static int et(Tag tag, String format, Object... args) {
        return et(tag, true, format, args);
    }

    public static int et(Tag tag, String msg) {
        return et(tag, true, msg);
    }

    public static int et(Tag tag, Throwable tr) {
        return et(tag, getStackTraceString(tr));
    }

    public static int et(Tag tag, String msg, Throwable tr) {
        return et(tag, "%s\n%s", msg, getStackTraceString(tr));
    }

    private static int et(Tag tag, boolean withStack, String format, Object... args) {
        if (loggerConfig.isPrintLoggable(LoggerConfig.ERROR)) {
            String message = null;
            if (args != null && args.length > 0) {
                message = String.format(format, args);
            } else {
                message = format;
            }
            return printLog(tag, LoggerConfig.ERROR, withStack, message);
        } else {
            return 0;
        }
    }

    public static int et(Tag tag, boolean withStack, String msg) {
        return printLog(tag, LoggerConfig.ERROR, withStack, msg);
    }

    // error with default tag
    public static int e(String format, Object... args) {
        return et(loggerConfig.getDefaultTag(), format, args);
    }

    public static int e(boolean withStack, String format, Object... args) {
        return et(loggerConfig.getDefaultTag(), withStack, format, args);
    }

    public static int e(String msg) {
        return et(loggerConfig.getDefaultTag(), true, msg);
    }

    public static int e(Throwable tr) {
        return et(loggerConfig.getDefaultTag(), tr);
    }

    public static int e(String msg, Throwable tr) {
        return et(loggerConfig.getDefaultTag(), msg, tr);
    }

    // dev with tag
    public static int devt(Tag tag, String format, Object... args) {
        return devt(tag, true, format, args);
    }

    public static int devt(Tag tag, String msg) {
        return devt(tag, true, msg);
    }

    public static int devt(Tag tag, Throwable tr) {
        return devt(tag, getStackTraceString(tr));
    }

    public static int devt(Tag tag, String msg, Throwable tr) {
        return devt(tag, "%s\n%s", msg, getStackTraceString(tr));
    }

    private static int devt(Tag tag, boolean withStack, String format, Object... args) {
        if (loggerConfig.isPrintLoggable(LoggerConfig.DEV)) {
            String message = null;
            if (args != null && args.length > 0) {
                message = String.format(format, args);
            } else {
                message = format;
            }
            return printLog(tag, LoggerConfig.DEV, withStack, message);
        } else {
            return 0;
        }
    }

    public static int devt(Tag tag, boolean withStack, String msg) {
        return printLog(tag, LoggerConfig.DEV, withStack, msg);
    }

    // dev with default tag.
    public static int dev(String format, Object... args) {
        return devt(loggerConfig.getDefaultTag(), format, args);
    }

    public static int dev(boolean withStack, String format, Object... args) {
        return devt(loggerConfig.getDefaultTag(), withStack, format, args);
    }

    public static int dev(String msg) {
        return devt(loggerConfig.getDefaultTag(), true, msg);
    }

    public static int dev(Throwable tr) {
        return devt(loggerConfig.getDefaultTag(), tr);
    }

    public static int dev(String msg, Throwable tr) {
        return devt(loggerConfig.getDefaultTag(), msg, tr);
    }
}
