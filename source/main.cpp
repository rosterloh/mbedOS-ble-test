#include "mbed-drivers/mbed.h"
#include "ble/BLE.h"
#include "EddystoneService.h"
#include "ble/services/DFUService.h"

EddystoneService *eddyServicePtr;
DFUService *dfuPtr;

/* Duration after power-on that config service is available. */
static const int CONFIG_ADVERTISEMENT_TIMEOUT_SECONDS = 30;

DigitalOut led(LED1, 1);

/**
 * Callback triggered upon a disconnection event.
 */
static void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *cbParams)
{
    (void) cbParams;
    BLE::Instance().gap().startAdvertising();
}

/**
 * Callback triggered some time after application started to switch to beacon mode.
 */
static void timeout(void)
{
    Gap::GapState_t state;
    state = BLE::Instance().gap().getState();
    if (!state.connected) { /* don't switch if we're in a connected state. */
        eddyServicePtr->startBeaconService(5, 5, 5);
    } else {
        minar::Scheduler::postCallback(timeout).delay(minar::milliseconds(CONFIG_ADVERTISEMENT_TIMEOUT_SECONDS * 1000));
    }
}

static void blinky(void)
{
    led = !led;
}

static void onBleInitError(BLE::InitializationCompleteCallbackContext* initContext)
{
    /* Initialization error handling goes here... */
    (void) initContext;
}

static void bleInitComplete(BLE::InitializationCompleteCallbackContext* initContext)
{
    BLE         &ble  = initContext->ble;
    ble_error_t error = initContext->error;

    if (error != BLE_ERROR_NONE) {
        onBleInitError(initContext);
        return;
    }

    ble.gap().onDisconnection(disconnectionCallback);

    dfuPtr = new DFUService(ble);

    /* Set UID and TLM frame data */
    const UIDNamespaceID_t uidNamespaceID = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
    const UIDInstanceID_t  uidInstanceID  = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint8_t tlmVersion = 0x00;

    /* Initialize a EddystoneBeaconConfig service providing config params, default URI, and power levels. */
    static const PowerLevels_t defaultAdvPowerLevels = {-47, -33, -21, -13}; // Values for ADV packets related to firmware levels, calibrated based on measured values at 1m
    static const PowerLevels_t radioPowerLevels      = {-30, -16, -4, 4};    // Values for radio power levels, provided by manufacturer.

    /* Set everything to defaults */
    eddyServicePtr = new EddystoneService(ble, defaultAdvPowerLevels, radioPowerLevels, 500);

    /* Set default URL, UID and TLM frame data if not initialized through the config service */
    eddyServicePtr->setURLData("http://mbed.org");
    eddyServicePtr->setUIDData(&uidNamespaceID, &uidInstanceID);
    eddyServicePtr->setTLMData(tlmVersion);

    /* Start Eddystone in config mode */
    eddyServicePtr->startConfigService();

    minar::Scheduler::postCallback(timeout).delay(minar::milliseconds(CONFIG_ADVERTISEMENT_TIMEOUT_SECONDS * 1000));
}

void app_start(int, char *[])
{
    /* Tell standard C library to not allocate large buffers for these streams */
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    setbuf(stdin, NULL);

    minar::Scheduler::postCallback(blinky).period(minar::milliseconds(500));

    BLE &ble = BLE::Instance();
    ble.init(bleInitComplete);
}
