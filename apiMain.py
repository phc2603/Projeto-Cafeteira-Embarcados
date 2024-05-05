import requests
import json
import qrcode
from GenerateAccessToken import GenerateAccessToken


class ApiMain:
    def __init__(self):
        pass

    def generateCharge(self, coffeValue):
        url = "https://pix.api.efipay.com.br/v2/cob"

        certificatePEM = "certificadoPRD.pem"

        headers = {
            'Authorization': f"Bearer {GenerateAccessToken().getAccessToken()}",
            'Content-Type': 'application/json'
        }

        body = {
            "calendario": {
                "expiracao": 3600
            },
            "valor": {
                "original": coffeValue
            },
            "chave": "07d967a0-9d71-47dd-8aa6-034781f95572",
            "solicitacaoPagador": "Cobrança dos serviços prestados."
        }
        try:
            response = requests.post(url, headers=headers, data=json.dumps(body), cert=certificatePEM).json()
            qrCodeImg = qrcode.make(response['pixCopiaECola'])
            type(qrCodeImg)
            qrCodeImg.save("qrCodePagamentoCafe.png")
            return True
        except Exception as e:
            return e

    def checkAccountValue(self):
        url = "https://pix.api.efipay.com.br/v2/gn/saldo"

        certificatePEM = "certificadoPRD.pem"

        headers = {
            'Authorization': f"Bearer {GenerateAccessToken().getAccessToken()}",
            'Content-Type': 'application/json'
        }

        try:
            response = requests.get(url, headers=headers, cert=certificatePEM).json()
            return float(response["saldo"])
        except Exception:
            return 0


