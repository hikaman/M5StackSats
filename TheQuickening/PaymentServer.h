class PaymentServer
{
  public:
   virtual boolean init();
   virtual String getServiceName();
   virtual PaymentInvoice getInvoice(int sats, String memo);
//   virtual WithdrawUrl getLNUrl(int sats, String memo);
   virtual bool isInvoicePaid(String id);
};
