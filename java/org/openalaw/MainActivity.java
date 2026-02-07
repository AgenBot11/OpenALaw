/**
 * OpenALaw Main Activity
 * 
 * Entry point for the OpenALaw Android application
 */

package org.openalaw;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;
import android.widget.Toast;

public class MainActivity extends Activity {
    private static final String TAG = "OpenALawMainActivity";
    private static final int REQUEST_ACCESSIBILITY = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        Log.i(TAG, "OpenALaw Main Activity created");
        
        // Check if accessibility service is enabled
        if (!isAccessibilityServiceEnabled()) {
            // Prompt user to enable accessibility service
            Toast.makeText(this, "Please enable OpenALaw Accessibility Service", Toast.LENGTH_LONG).show();
            startActivity(new Intent(Settings.ACTION_ACCESSIBILITY_SETTINGS));
        } else {
            Toast.makeText(this, "OpenALaw Service is active", Toast.LENGTH_SHORT).show();
        }
        
        // Finish the activity as the service runs in background
        finish();
    }

    /**
     * Check if the accessibility service is enabled
     */
    private boolean isAccessibilityServiceEnabled() {
        try {
            int enabled = Settings.Secure.getInt(getContentResolver(), 
                Settings.Secure.ACCESSIBILITY_ENABLED);
            if (enabled == 1) {
                String prefString = Settings.Secure.getString(getContentResolver(), 
                    Settings.Secure.ENABLED_ACCESSIBILITY_SERVICES);
                if (prefString != null) {
                    return prefString.contains(getPackageName() + "/" + OpenALawService.class.getCanonicalName());
                }
            }
        } catch (Settings.SettingNotFoundException e) {
            Log.e(TAG, "Accessibility setting not found", e);
        }
        return false;
    }
}