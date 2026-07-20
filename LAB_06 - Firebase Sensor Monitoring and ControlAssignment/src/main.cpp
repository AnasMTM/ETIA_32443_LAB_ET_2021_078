#define ENABLE_DATABASE

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// -----------------------------------------------------------------------------
// WiFi Credentials
// -----------------------------------------------------------------------------
#define WIFI_SSID     "Anas"
#define WIFI_PASSWORD "Anas1234"

// -----------------------------------------------------------------------------
// Firebase Configuration
// -----------------------------------------------------------------------------
#define DATABASE_URL "https://lab-06-b392e-default-rtdb.firebaseio.com/"

void asyncCB(AsyncResult &aResult);

NoAuth no_auth;
FirebaseApp app;

WiFiClientSecure ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

RealtimeDatabase Database;

// -----------------------------------------------------------------------------
// LDR Configuration
// -----------------------------------------------------------------------------
#define LDR_PIN A0

unsigned long lastSendMillis = 0;
const unsigned long sendInterval = 5000;   // Send every 5 seconds

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    Serial.println();

    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }

    Serial.println();
    Serial.print("Connected, IP: ");
    Serial.println(WiFi.localIP());

    // Allow insecure SSL connection
    ssl_client.setInsecure();

    // Initialize Firebase
    Serial.println("Initializing Firebase...");

    initializeApp(
        aClient,
        app,
        getAuth(no_auth),
        asyncCB,
        "authTask"
    );

    app.getApp(Database);
    Database.url(DATABASE_URL);
}

// -----------------------------------------------------------------------------
// Main Loop
// -----------------------------------------------------------------------------
void loop()
{
    app.loop();
    Database.loop();

    if (app.ready() &&
        (millis() - lastSendMillis >= sendInterval))
    {
        lastSendMillis = millis();

        // Read LDR value
        int ldrValue = analogRead(LDR_PIN);

        Serial.print("LDR Value: ");
        Serial.println(ldrValue);

        // Upload LDR value to Firebase
        Database.set(
            aClient,
            "/sensor/LDR",
            ldrValue,
            asyncCB,
            "LDRTask"
        );
    }
}

// -----------------------------------------------------------------------------
// Firebase Async Callback
// -----------------------------------------------------------------------------
void asyncCB(AsyncResult &aResult)
{
    if (aResult.isEvent())
    {
        Serial.printf(
            "Event [%s]: %s (Code %d)\n",
            aResult.uid().c_str(),
            aResult.eventLog().message().c_str(),
            aResult.eventLog().code()
        );
    }

    if (aResult.isError())
    {
        Serial.printf(
            "Error [%s]: %s (Code %d)\n",
            aResult.uid().c_str(),
            aResult.error().message().c_str(),
            aResult.error().code()
        );
    }

    if (aResult.available())
    {
        Serial.printf("Response: %s\n", aResult.c_str());
    }
}