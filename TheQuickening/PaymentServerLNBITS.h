class PaymentServerLNbits : public PaymentServer
{
  public:
   virtual boolean init();
   virtual String getServiceName();
   virtual PaymentInvoice getInvoice(int sats, String memo);
  // virtual WithdrawUrl getLNUrl(int sats, String memo); 
   virtual bool isInvoicePaid(String id);
  private:
   String Server;
   int Port;
   String InvoiceKey;
   String AdminKey;
   String api_endpoint;
   String invoice_endpoint;
   String withdraw_endpoint;
   String tlscert;
   String createRequest(String method, String path, String data);
};
