
package com.kakao.util.helper.log;

import android.util.Log;

import java.util.HashSet;
import java.util.Set;

public class LoggerConfig {

    private int printLoggerLevel;

    private Tag defaultTag;

    private String stackPrefix;

    private Set<String> ignoreStackSet;

    public static final int DEV = 0;

    public static final int VERBOSE = Log.VERBOSE;

    public static final int DEBUG = Log.DEBUG;

    public static final int INFO = Log.INFO;

    public static final int WARN = Log.WARN;

    public static final int ERROR = Log.ERROR;

    public static final int ASSERT = Log.ASSERT;

    private String getTraceInfo() {
        StackTraceElement[] stacks = Thread.currentThread().getStackTrace();
        return getTraceInfo(stacks);
    }

    private String getTraceInfo(StackTraceElement[] stacks) {
        StackTraceElement stack = null;
        String loggerName = this.getClass().getCanonicalName();
        String className = null;
        for (StackTraceElement stack1 : stacks) {
            className = stack1.getClassName();
            if (ignoreStackSet.contains(className)) {
                continue;
            }
            if (!className.startsWith(loggerName) && (stackPrefix == null || className.startsWith(stackPrefix))) {
                stack = stack1;
                break;
            }
        }

        if (stack == null) {
            return null;
        }

        String klass = stack.getFileName();
        String method = stack.getMethodName();
        int line = stack.getLineNumber();
        return String.format("[%s:%s():%d]", klass, method, line);
    }

    public Tag getDefaultTag() {
        return defaultTag;
    }

    private String getMessageWithTrace(String message) {
        return String.format("%s %s", getTraceInfo(), message);
    }

    public static String toSimpleStringLogLevel(int level) {
        switch (level) {
            case DEV:
                return "DEV";
            case VERBOSE:
                return "V";
            case DEBUG:
                return "D";
            case INFO:
                return "I";
            case WARN:
                return "W";
            case ERROR:
                return "E";
            case ASSERT:
                return "A";
        }

        return "NONE";
    }

    public boolean isPrintLoggable(int level) {
        return (level >= printLoggerLevel);
    }

    public static class Builder {

        private int printLoggerLevel = DEV;

        private Tag defaultTag = Tag.DEFAULT;

        private String stackPrefix;

        private Set<String> ignoreStackSet = new HashSet<String>();

        public Builder setDefaultTag(Tag tag) {
            defaultTag = tag;
            return this;
        }

        public Builder setPrintLoggerLevel(int level) {
            printLoggerLevel = level;
            return this;
        }

        public Builder setIgnoreSet(Set<String> set) {
            if (set == null) {
                return this;
            }
            ignoreStackSet = set;
            return this;
        }

        public Builder setStackPrefix(String prefix) {
            stackPrefix = prefix;
            return this;
        }

        public LoggerConfig build() {
            LoggerConfig loggerConfig = new LoggerConfig();
            loggerConfig.defaultTag = defaultTag;
            loggerConfig.printLoggerLevel = printLoggerLevel;
            loggerConfig.stackPrefix = stackPrefix;
            loggerConfig.ignoreStackSet = ignoreStackSet;
            return loggerConfig;
        }
    }


    public String getMessage(boolean withStack, String msg) {
        return withStack ? getMessageWithTrace(msg) : msg;
    }
}
