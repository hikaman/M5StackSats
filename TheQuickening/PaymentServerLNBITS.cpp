#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "PaymentInvoice.h"
#include "PaymentServer.h"
#include "PaymentServerLNBITS.h"

#include "config.h"

/*
String Server;
String InvoiceKey;
String AdminKey;
String invoice_create_endpoint;
String invoice_check_endpoint;
String withdraw_check_endpoint;
String tlscert;
*/

String PaymentServerLNbits::getServiceName() { return (String)"LNBITS"; };

bool PaymentServerLNbits::init() {

    // Endpoint Setup
    PaymentServerLNbits::Server = LNBitsServer; // domain or IP of LNBits
    PaymentServerLNbits::Port = LNBitsPort; // default is 5000
    PaymentServerLNbits::InvoiceKey = LNBitsInvoiceApiKey;
    PaymentServerLNbits::AdminKey = LNBitsAdminApiKey;
    PaymentServerLNbits::tlscert= ""; // ignore for now
    PaymentServerLNbits::api_endpoint = PaymentServerLNbits::Server+":"+String(PaymentServerLNbits::Port);
    PaymentServerLNbits::invoice_endpoint = "/api/v1/payments";
    return true;
}

/*
Create an invoice (incoming):

POST /api/v1/payments

Headers:
{"X-Api-Key": "<INVOICE API KEY>"}

Body (application/json):
{"out": false, "amount": <int>, "memo": <string>}

Returns 201 CREATED (application/json):
{"checking_id": <string>, "payment_request": <string>}
*/

PaymentInvoice PaymentServerLNbits::getInvoice(int sats, String memo) { 
    String toPost = "{ \"out\" : false, \"amount\" :" + (String) sats +", \"memo\" :\""+ memo + String(random(1,1000)) + "\"}";

    String payload = createRequest((String)"POST",invoice_endpoint,toPost);

    const size_t capacity = JSON_OBJECT_SIZE(2) + 500;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, payload);
    Serial.println(payload);

    const char* payment_request = doc["payment_request"]; 
    const char* id = doc["id"]; 
    String payreq = (String) payment_request;
    String lntx_id = (String) id;
    Serial.println(payreq);
    Serial.println(lntx_id);

    PaymentInvoice invoice;
    invoice.paymentRequest = payreq;
    invoice.id = lntx_id;
    return invoice;
}




bool PaymentServerLNbits::isInvoicePaid(String id) { 

   
    String payloadd = createRequest((String)"GET",invoice_endpoint,(String)"");
    delay(1500);
    
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payloadd);
    int Settled = doc["settled"];
    Serial.println(Settled);

    if (Settled == 1){
      return true;
    } else{
      return false;
    }

}

// PRIVATE METHODS

String PaymentServerLNbits::createRequest(String method, String path, String data)
  {
    String payload;
    int httpCode;
    HTTPClient http;

    Serial.println("PaymentConnector::createRequest BEGIN-------");
    Serial.println("METHOD:" + method);
    Serial.println("URL:" + PaymentServerLNbits::api_endpoint + path);
    Serial.println("DATA:" + data);
    
    http.begin(PaymentServerLNbits::api_endpoint + path); //Getting fancy to response size
    http.addHeader("Content-Type","application/json");
    http.addHeader("X-Api-Key",PaymentServerLNbits::InvoiceKey);
    
    Serial.println("HEADER:" + PaymentServerLNbits::InvoiceKey);
    
    
    if (method.equals("POST"))
      httpCode = http.POST(data); //Make the request
    else if (method.equals("GET")) {
      httpCode = http.GET(); //Make the request
    } else {
      Serial.println("This HTTP method usage is not defined");
    }

    if (httpCode > 0) { //Check for the returning code
      payload = http.getString();
      Serial.println("RESPONSE:" + payload);
    } else {
      Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources
    Serial.println("PaymentConnector::createRequest END--------");
   
    return payload;
  }
