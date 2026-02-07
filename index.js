/**
 * OpenALaw Main Entry Point
 * 
 * The Future of Free-Will AI on Android
 * 
 * This is the main entry point for the OpenALaw autonomous AI agent
 * designed specifically for ARM-based Android devices.
 */

const OpenALawCore = require('./core.js');
const AndroidCoreBridge = require('./android-core-bridge.js');

class OpenALaw {
  constructor(config = {}) {
    this.config = config;
    this.version = '1.0.0';
    this.architecture = 'ARM';
    this.platform = 'Android';
    this.agentId = 'AgentBot11';
    
    // Initialize core components
    this.core = null;
    this.bridge = null;
    
    console.log(`OpenALaw v${this.version} initialized for ${this.architecture} architecture`);
  }

  /**
   * Initialize the complete OpenALaw system
   */
  async initialize() {
    console.log('Initializing OpenALaw system...');
    
    // Initialize core AI agent
    this.core = new OpenALawCore(this.config);
    await this.core.initialize();
    
    // Initialize Android system bridge
    this.bridge = new AndroidCoreBridge();
    await this.bridge.initialize();
    
    console.log('OpenALaw system fully initialized!');
    
    // Log system status
    this.reportStatus();
    
    return true;
  }

  /**
   * Process a task through the OpenALaw system
   */
  async processTask(task) {
    console.log(`Processing task: ${task}`);
    
    // First try to process with core AI
    const coreResult = await this.core.processTask(task);
    
    // If the task involves Android system interaction, use the bridge
    if (this.requiresSystemInteraction(task)) {
      console.log('Task requires Android system interaction');
      const systemResult = await this.handleSystemTask(task);
      return {
        core: coreResult,
        system: systemResult,
        combined: true
      };
    }
    
    return coreResult;
  }

  /**
   * Check if a task requires Android system interaction
   */
  requiresSystemInteraction(task) {
    const systemKeywords = [
      'screen', 'touch', 'click', 'swipe', 'tap', 'gesture', 
      'app', 'application', 'interface', 'ui', 'view', 'input',
      'keyboard', 'navigation', 'activity', 'window', 'notification'
    ];
    
    const taskLower = task.toLowerCase();
    return systemKeywords.some(keyword => taskLower.includes(keyword));
  }

  /**
   * Handle tasks that require Android system interaction
   */
  async handleSystemTask(task) {
    console.log(`Handling system task: ${task}`);
    
    // Example system interactions
    if (task.toLowerCase().includes('screen')) {
      return await this.bridge.captureScreen();
    } else if (task.toLowerCase().includes('touch') || task.toLowerCase().includes('click')) {
      // Parse coordinates from task (simplified)
      const coords = this.extractCoordinates(task);
      if (coords) {
        return await this.bridge.tap(coords.x, coords.y);
      }
    }
    
    return { success: false, reason: 'System task not implemented yet' };
  }

  /**
   * Extract coordinates from a task description
   */
  extractCoordinates(task) {
    // Simple regex to extract coordinates from task
    const coordRegex = /(\d+)\D+(\d+)/g;
    const matches = [...task.matchAll(coordRegex)];
    
    if (matches.length > 0) {
      const [_, x, y] = matches[0];
      return { x: parseInt(x), y: parseInt(y) };
    }
    
    return null;
  }

  /**
   * Report system status
   */
  reportStatus() {
    const coreStatus = this.core ? this.core.getStatus() : { initialized: false };
    const bridgeStatus = this.bridge ? this.bridge.getStatus() : { initialized: false };
    
    console.log('\n=== OpenALaw System Status ===');
    console.log(`Version: ${this.version}`);
    console.log(`Architecture: ${this.architecture}`);
    console.log(`Platform: ${this.platform}`);
    console.log(`Agent ID: ${this.agentId}`);
    console.log(`Core Initialized: ${coreStatus.initialized}`);
    console.log(`Bridge Initialized: ${bridgeStatus.initialized}`);
    console.log(`Timestamp: ${new Date().toISOString()}`);
    console.log('==============================\n');
  }

  /**
   * Get overall system status
   */
  getStatus() {
    return {
      version: this.version,
      architecture: this.architecture,
      platform: this.platform,
      agentId: this.agentId,
      core: this.core ? this.core.getStatus() : null,
      bridge: this.bridge ? this.bridge.getStatus() : null,
      timestamp: new Date().toISOString()
    };
  }
}

// Export the main class
module.exports = OpenALaw;

// For direct execution
if (require.main === module) {
  (async () => {
    console.log('Starting OpenALaw (AgentBot11)...');
    console.log('The Future of Free-Will AI on Android');
    
    const openalaw = new OpenALaw();
    await openalaw.initialize();
    
    // Example task processing
    console.log('\nExample: Processing a simple task...');
    const result = await openalaw.processTask('Analyze this input');
    console.log('Result:', result);
    
    console.log('\nOpenALaw system ready for tasks!');
  })();
}