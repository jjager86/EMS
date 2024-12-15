
// **********************************
// *  MDNS discovery service   *
// **********************************

void _mdns()
{
    Serial.println(F("Starting MDNS responder service"));

    bool mdns_result = MDNS.begin(HOSTNAME);
    if (mdns_result)
    {
        MDNS.addService("http", "tcp", 80);
    }
}
