import requests
import base64
import os


class GenerateAccessToken:
    def __init__(self):
        pass

    def getAccessToken(self):
        credentials = {
            "client_id": f"CLIENT ID",
            "client_secret": f"CLIENT SECRET",
        }

        certificatePEM = "certificadoPRD.pem"

        auth = base64.b64encode(
            (f"{credentials['client_id']}:{credentials['client_secret']}"
             ).encode()).decode()

        url = "https://pix.api.efipay.com.br/oauth/token"

        payload = "{\r  \n\"grant_type\": \"client_credentials\"\r\n}"
        headers = {
            'Authorization': f"Basic {auth}",
            'Content-Type': 'application/json'
        }

        try:
            response = requests.post(url,
                                     headers=headers,
                                     data=payload,
                                     cert=certificatePEM)
            if response.status_code == 200:
                return response.json()['access_token']
            else:
                raise "Error"
        except Exception:
            return None

