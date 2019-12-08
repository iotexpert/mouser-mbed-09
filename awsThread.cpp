#include "mbed.h"
#include "aws_client.h"
#include "aws_config.h"
#include "temperatureThread.h"


extern WiFiInterface *wifi;

void messageArrived(aws_iot_message_t& md)
{
    float setPoint; 
    aws_message_t &message = md.message;
    sscanf((char*)message.payload,"%f",&setPoint);
    tempSendUpdateCurrentSetPointF(setPoint);
}

void awsThread(void)
{
    AWSIoTClient client;
    AWSIoTEndpoint* ep = NULL;

    /* Initialize AWS Client library */
    AWSIoTClient AWSClient(wifi, "thermostat" , SSL_CLIENTKEY_PEM, strlen(SSL_CLIENTKEY_PEM), SSL_CLIENTCERT_PEM, strlen(SSL_CLIENTCERT_PEM));

    aws_connect_params conn_params = { 0,0,NULL,NULL,NULL,NULL,NULL };
    ep = AWSClient.create_endpoint(AWS_TRANSPORT_MQTT_NATIVE, AWSIOT_ENDPOINT_ADDRESS, 8883, SSL_CA_PEM, strlen(SSL_CA_PEM));

    /* set MQTT connection parameters */
    conn_params.username = NULL;
    conn_params.password = NULL;
    conn_params.keep_alive = 60;
    conn_params.peer_cn = (uint8_t*) AWSIOT_ENDPOINT_ADDRESS;
    conn_params.client_id = (uint8_t*)"thermostat";

    /* connect to an AWS endpoint */
    AWSClient.connect (ep, conn_params);
 
    AWSClient.subscribe(ep, "setPoint", AWS_QOS_ATMOST_ONCE, messageArrived);
    aws_publish_params publish_params = { AWS_QOS_ATMOST_ONCE };
    
    bool doPublish=false;
    float currentTemp;
    while(1)
    {
        AWSClient.yield(1000);
    }
}
