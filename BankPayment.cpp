#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <iomanip>
#include <sstream>

using namespace std;

string formatVND(double value) {
    long long rounded = llround(value);
    bool negative = rounded < 0;
    string s = to_string(negative ? -rounded : rounded);
    string result;
    int count = 0;
    for (int i = static_cast<int>(s.length()) - 1; i >= 0; --i) {
        if (count > 0 && count % 3 == 0) {
            result = "," + result;
        }
        result = s[i] + result;
        count++;
    }
    if (negative) {
        result = "-" + result;
    }
    return result + " VND";
}

class BankAccountPayment {
protected:
    string bankCode;
    string bankName;
    string accountNumber;
    double balance;

public:
    BankAccountPayment(const string& accountNumber, double balance)
        : bankCode(""), bankName(""), accountNumber(accountNumber), balance(balance) {}

    BankAccountPayment(const string& bankCode, const string& bankName, const string& accountNumber, double balance)
        : bankCode(bankCode), bankName(bankName), accountNumber(accountNumber), balance(balance) {}

    virtual ~BankAccountPayment() = default;

    string getBankCode() const {
        return bankCode;
    }

    string getBankName() const {
        return bankName;
    }

    string getAccountNumber() const {
        return accountNumber;
    }

    double getBalance() const {
        return balance;
    }

    virtual double calculateFee(double amount) const = 0;

    virtual bool pay(double amount) {
        cout << "\n--- XU LY GIAO DICH ---\n";
        cout << "Ngan hang: " << bankName << "\n";
        cout << "So tien: " << formatVND(amount) << "\n";

        if (amount <= 0) {
            cout << "-> GIAO DICH THAT BAI: So tien khong hop le!\n";
            return false;
        }

        double fee = calculateFee(amount);
        double total = amount + fee;

        cout << "Phi giao dich: " << formatVND(fee) << "\n";
        cout << "Tong tien tru: " << formatVND(total) << "\n";

        if (balance >= total) {
            balance -= total;
            cout << "-> THANH TOAN THANH CONG!\n";
            cout << "So du con lai: " << formatVND(balance) << "\n";
            return true;
        } else {
            cout << "-> GIAO DICH THAT BAI: So du khong du!\n";
            cout << "So du hien tai: " << formatVND(balance) << "\n";
            return false;
        }
    }
};

class VCBPayment : public BankAccountPayment {
public:
    VCBPayment(const string& accountNumber, double balance)
        : BankAccountPayment("VCB", "Vietcombank", accountNumber, balance) {}

    double calculateFee(double amount) const override {
        return 1100.0;
    }
};

class TCBPayment : public BankAccountPayment {
public:
    TCBPayment(const string& accountNumber, double balance)
        : BankAccountPayment("TCB", "Techcombank", accountNumber, balance) {}

    double calculateFee(double amount) const override {
        return 0.0;
    }
};

class PaymentGateway {
private:
    vector<shared_ptr<BankAccountPayment>> paymentMethods;

public:
    void addPaymentMethod(shared_ptr<BankAccountPayment> method) {
        if (method) {
            paymentMethods.push_back(method);
        }
    }

    void displaySupportedBanks() const {
        cout << "=== CONG THANH TOAN NGAN HANG ===\n";
        for (size_t i = 0; i < paymentMethods.size(); ++i) {
            cout << "[" << (i + 1) << "] "
                 << paymentMethods[i]->getBankName() << " (" << paymentMethods[i]->getBankCode() << ")"
                 << " - So tai khoan: " << paymentMethods[i]->getAccountNumber()
                 << " | So du: " << formatVND(paymentMethods[i]->getBalance()) << "\n";
        }
    }

    shared_ptr<BankAccountPayment> selectBank(int index) const {
        if (index < 1 || index > static_cast<int>(paymentMethods.size())) {
            return nullptr;
        }
        return paymentMethods[index - 1];
    }

    size_t getBankCount() const {
        return paymentMethods.size();
    }
};

class MBBankPayment : public BankAccountPayment {
public:
    MBBankPayment(const string& accountNumber, double balance)
        : BankAccountPayment("MBB", "MBBank", accountNumber, balance) {}

    double calculateFee(double amount) const override {
        return amount * 0.002;
    }
};

int main() {
    PaymentGateway gateway;

    gateway.addPaymentMethod(make_shared<VCBPayment>("0011001234567", 500000));
    gateway.addPaymentMethod(make_shared<TCBPayment>("1903567890123", 300000));
    gateway.addPaymentMethod(make_shared<MBBankPayment>("0987654321", 1000000));

    gateway.displaySupportedBanks();

    int choice;
    cout << "Chon ngan hang (1-" << gateway.getBankCount() << "): ";
    if (!(cin >> choice)) {
        return 0;
    }

    shared_ptr<BankAccountPayment> selectedBank = gateway.selectBank(choice);
    if (!selectedBank) {
        cout << "Lua chon ngan hang khong hop le!\n";
        return 0;
    }

    double amount;
    cout << "\nNhap so tien can thanh toan: ";
    if (!(cin >> amount)) {
        return 0;
    }

    selectedBank->pay(amount);

    return 0;
}
