#ifndef BLOCKYTALKY_KEYVALUE_SERVICE_H
#define BLOCKYTALKY_KEYVALUE_SERVICE_H

#include "MicroBitConfig.h"
#include "MicroBitThermometer.h"
#include "EventModel.h"
#include "pxt.h"


#define BLOCKYTALKY_KV_ID 9600
#define BLOCKYTALKY_KV_RECEIVED_NUMBER 1
#define BLOCKYTALKY_KV_RECEIVED_STRING 2

#define BLOCKYTALKY_KV_KEY_LENGTH 6
#define BLOCKYTALKY_KV_VALUE_LENGTH 12

// UUIDs for our service and characteristics
extern const uint8_t  KeyValueServiceUUID[];
extern const uint8_t  KeyValueTxCharacteristicUUID[];
extern const uint8_t  KeyValueRxCharacteristicUUID[];

// 20 bytes
struct KeyValueMessage {
  char key[BLOCKYTALKY_KV_KEY_LENGTH + 1]; // 7
  uint8_t type; // 1
  uint8_t value[BLOCKYTALKY_KV_VALUE_LENGTH]; // 12
};


enum BlockyTalkyMessageType {
    //%
    Int32LE = 1,
    //%
    Float64LE,
    //%
    StringType
};

//================================================================
#if MICROBIT_CODAL
//================================================================

#include "MicroBitBLEManager.h"
#include "MicroBitBLEService.h"

class KeyValueService : public MicroBitBLEService
{
    // Index for each charactersitic in arrays of handles and UUIDs
    typedef enum mbbs_cIdx
    {
        mbbs_cIdxTX,
        mbbs_cIdxRX,
        mbbs_cIdxCOUNT
    } mbbs_cIdx;
    
    // Data for each characteristic when they are held by Soft Device.
    MicroBitBLEChar chars[ mbbs_cIdxCOUNT];

    public:
    
    int              characteristicCount()          { return mbbs_cIdxCOUNT; };
    MicroBitBLEChar *characteristicPtr( int idx)    { return &chars[ idx]; };

    public:

    typedef microbit_ble_evt_write_t dataWritten_t;

    private:

    void notifyTX() { notifyChrValue(mbbs_cIdxMESSAGE,(uint8_t *)&txCharacteristicMessage, sizeof(txCharacteristicMessage)); }

    microbit_charhandle_t handleRX() { return valueHandle( mbbs_cIdxRX); }

//================================================================
#else // MICROBIT_CODAL
//================================================================

#include "ble/BLE.h"

class KeyValueService
{
    // Handles to access each characteristic when they are held by Soft Device.
    GattAttribute::Handle_t txCharacteristicHandle;
    GattAttribute::Handle_t rxCharacteristicHandle;

    public:

    typedef GattWriteCallbackParams dataWritten_t;

    private:

    void notifyTX() { ble.gattServer().notify(txCharacteristicHandle,(uint8_t *)&txCharacteristicMessage, sizeof(txCharacteristicMessage)); }

    bool getConnected() { return ble.getGapState().connected; }

    GattAttribute::Handle_t handleRX() { return rxCharacteristicHandle; }

//================================================================
#endif // MICROBIT_CODAL
//================================================================

    public:

    /**
      * Constructor.
      * Create a representation of the TemperatureService
      * @param _ble The instance of a BLE device that we're running on.
      */
    KeyValueService(BLEDevice &_ble);

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten(const dataWritten_t *params);

    /**
    * Sends a key value pair
    */
    void send(String key, int type, Buffer value);

    /*
      Gets the last message
    */
    KeyValueMessage receivedMessage();

    private:

    // Bluetooth stack we're running on.
    BLEDevice &ble;

    // memory for buffers.
    KeyValueMessage txCharacteristicMessage;
    KeyValueMessage rxCharacteristicMessage;
};

#endif