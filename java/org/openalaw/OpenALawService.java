/**
 * OpenALaw Android Service
 * 
 * Main Android service that connects the Java/Kotlin layer with the native C++ layer
 * for the OpenALaw AI agent on ARM architecture.
 */

package org.openalaw;

import android.accessibilityservice.AccessibilityService;
import android.accessibilityservice.AccessibilityServiceInfo;
import android.content.Context;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.Build;
import android.util.Log;
import android.view.Gravity;
import android.view.WindowManager;
import android.view.accessibility.AccessibilityEvent;
import android.view.accessibility.AccessibilityNodeInfo;
import android.accessibilityservice.GestureDescription;
import android.graphics.Path;

import java.util.List;

public class OpenALawService extends AccessibilityService {
    private static final String TAG = "OpenALawService";
    
    // Native interface methods
    private static native boolean nativeInitialize();
    private static native Object nativeCaptureScreen();
    private static native boolean nativeSimulateTouch(int x, int y, int action);
    private static native void nativeSetAccessibilityService(Object service);

    static {
        try {
            System.loadLibrary("android-bridge");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to load native library", e);
        }
    }

    @Override
    public void onServiceConnected() {
        super.onServiceConnected();
        Log.i(TAG, "OpenALaw Service Connected");
        
        // Initialize native components
        boolean initSuccess = nativeInitialize();
        Log.i(TAG, "Native initialization: " + initSuccess);
        
        // Register this service with native layer
        nativeSetAccessibilityService(this);
        
        // Configure accessibility service settings
        configureAccessibilityService();
    }

    @Override
    public void onAccessibilityEvent(AccessibilityEvent event) {
        Log.d(TAG, "Accessibility event received: " + event.getEventType());
        
        // Forward events to native layer for AI processing
        handleAccessibilityEvent(event);
    }

    @Override
    public void onInterrupt() {
        Log.d(TAG, "Accessibility service interrupted");
    }

    /**
     * Configure accessibility service settings
     */
    private void configureAccessibilityService() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
            AccessibilityServiceInfo info = getServiceInfo();
            info.eventTypes = AccessibilityEvent.TYPES_ALL_MASK;
            info.feedbackType = AccessibilityServiceInfo.FEEDBACK_GENERIC;
            info.flags |= AccessibilityServiceInfo.FLAG_REQUEST_TOUCH_EXPLORATION_MODE |
                         AccessibilityServiceInfo.FLAG_REQUEST_ENHANCED_WEB_ACCESSIBILITY |
                         AccessibilityServiceInfo.FLAG_REPORT_VIEW_IDS |
                         AccessibilityServiceInfo.FLAG_REQUEST_FILTER_KEY_EVENTS;
            setServiceInfo(info);
        }
    }

    /**
     * Handle accessibility events and forward to AI core
     */
    private void handleAccessibilityEvent(AccessibilityEvent event) {
        // Process the event based on type
        switch (event.getEventType()) {
            case AccessibilityEvent.TYPE_VIEW_CLICKED:
                Log.d(TAG, "View clicked: " + event.getText());
                break;
            case AccessibilityEvent.TYPE_VIEW_FOCUSED:
                Log.d(TAG, "View focused: " + event.getText());
                break;
            case AccessibilityEvent.TYPE_WINDOW_CONTENT_CHANGED:
                Log.d(TAG, "Window content changed");
                break;
            default:
                Log.d(TAG, "Other event: " + event.getEventType());
        }
    }

    /**
     * Public method to capture screen content
     */
    public Object captureScreen() {
        return nativeCaptureScreen();
    }

    /**
     * Public method to simulate touch at coordinates
     */
    public boolean simulateTouch(int x, int y) {
        return nativeSimulateTouch(x, y, 0); // 0 for tap action
    }

    /**
     * Simulate swipe gesture
     */
    public boolean simulateSwipe(int startX, int startY, int endX, int endY) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            Path path = new Path();
            path.moveTo(startX, startY);
            path.lineTo(endX, endY);
            
            GestureDescription.Builder builder = new GestureDescription.Builder();
            builder.addStroke(new GestureDescription.StrokeDescription(path, 0, 500));
            
            return dispatchGesture(builder.build(), null, null);
        }
        return false; // Not supported on older versions
    }

    /**
     * Find UI element by text
     */
    public AccessibilityNodeInfo findElementByText(String text) {
        AccessibilityNodeInfo rootNode = getRootInActiveWindow();
        if (rootNode != null) {
            List<AccessibilityNodeInfo> nodes = rootNode.findAccessibilityNodeInfosByText(text);
            if (!nodes.isEmpty()) {
                return nodes.get(0);
            }
        }
        return null;
    }

    /**
     * Find UI element by view ID
     */
    public AccessibilityNodeInfo findElementById(String resourceId) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
            AccessibilityNodeInfo rootNode = getRootInActiveWindow();
            if (rootNode != null) {
                List<AccessibilityNodeInfo> nodes = rootNode.findAccessibilityNodeInfosByViewId(resourceId);
                if (!nodes.isEmpty()) {
                    return nodes.get(0);
                }
            }
        }
        return null;
    }
}