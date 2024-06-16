import tkinter
from apiMain import ApiMain
from tkinter import messagebox
import pika

GENERAL_BACKGROUND_COLOR = "#967259"


class RabbitQueue:
    def __init__(self):
        self.connection = pika.BlockingConnection(pika.ConnectionParameters("172.20.10.5"))
        self.channel = self.connection.channel()
        self.routingQueueCoffe = "coffe_machine"
        self.channel.queue_declare(queue=self.routingQueueCoffe)

    def sendToQueue(self, coffeSize):
        self.channel.basic_publish(exchange='',
                                   routing_key=self.routingQueueCoffe,
                                   body=f"{coffeSize}")


class GUI:
    def __init__(self, rabbitClass):
        self.rabbitClass = rabbitClass
        self.screen = tkinter.Tk()
        self.screen.title("Cafeteira Embarcados")
        self.screen.config(padx=5, pady=9, bg=GENERAL_BACKGROUND_COLOR)
        self.screen.geometry("800x500")
        self.qrCodeImage = None

        self.label_title = tkinter.Label(text='Escolha o tamanho do café', font=("Arial", 20, "bold"), fg="#ece0d1",
                                         bg=GENERAL_BACKGROUND_COLOR)
        self.label_title.place(x=200, y=50)

        smallCoffeImage = tkinter.PhotoImage(file="small.png")

        self.smallCoffeButton = tkinter.Button(self.screen, image=smallCoffeImage, fg="#454545", bg="#FFF4E0",
                                               command=lambda: self.changeScreenToQrCode("1.00", "S"))
        self.smallCoffeButton.place(x=75, y=225)
        self.label_small = tkinter.Label(text='S', font=("Arial", 20, "bold"), fg="#ece0d1",
                                         bg=GENERAL_BACKGROUND_COLOR)
        self.label_small.place(x=100, y=360)

        mediumCoffeImage = tkinter.PhotoImage(file="medium.png")
        self.mediumCoffeButton = tkinter.Button(self.screen, image=mediumCoffeImage, fg="#454545", bg="#FFF4E0",
                                                command=lambda: self.changeScreenToQrCode("3.00", "M"))
        self.mediumCoffeButton.place(x=335, y=215)
        self.label_medium = tkinter.Label(text='M', font=("Arial", 20, "bold"), fg="#ece0d1",
                                          bg=GENERAL_BACKGROUND_COLOR)
        self.label_medium.place(x=365, y=360)

        largeCoffeImage = tkinter.PhotoImage(file="large.png")
        self.largeCoffeButton = tkinter.Button(self.screen, image=largeCoffeImage, fg="#454545", bg="#FFF4E0",
                                               command=lambda: self.changeScreenToQrCode("5.00", "L"))
        self.largeCoffeButton.place(x=585, y=205)
        self.label_large = tkinter.Label(text='L', font=("Arial", 20, "bold"), fg="#ece0d1",
                                         bg=GENERAL_BACKGROUND_COLOR)
        self.label_large.place(x=625, y=360)

        self.screen.mainloop()

    def changeScreenToQrCode(self, coffeValue, size):
        charge = ApiMain()
        previousValueAccount = charge.checkAccountValue()
        currentCharge = charge.generateCharge(coffeValue)
        if currentCharge:
            self.label_title.destroy()
            self.smallCoffeButton.destroy()
            self.mediumCoffeButton.destroy()
            self.largeCoffeButton.destroy()
            self.label_small.destroy()
            self.label_medium.destroy()
            self.label_large.destroy()

            self.createQrCode(previousValueAccount, charge, size)
        else:
            messagebox.showerror(title="Error",
                                 message="Ocorreu um erro ao ir para o pagamento. Favor tente novamente mais tarde")

    def createQrCode(self, previousValueAccount, charge, size):
        qrCodeTitle = tkinter.Label(self.screen, text="Escaneie o qrCode no App bancário para realizar o pagamento",
                                    font=("Arial", 12, "bold"), fg="#ece0d1",
                                    bg=GENERAL_BACKGROUND_COLOR)
        qrCodeTitle.place(x=125, y=25)

        self.qrCodeImage = tkinter.PhotoImage(file="qrCodePagamentoCafe.png")
        panel = tkinter.Label(self.screen, image=self.qrCodeImage, fg="#454545", bg="#FFF4E0")
        panel.place(x=220, y=90)

        buttonConfirmPayment = tkinter.Button(self.screen, text="Confirmar pagamento", font=("Arial", 13, "bold"),
                                              fg="#454545", bg="#FFF4E0",
                                              command=lambda: self.checkPayment(previousValueAccount, charge, size))
        buttonConfirmPayment.place(x=540, y=410)

        buttonCancelTransaction = tkinter.Button(self.screen, text="Cancelar transação", font=("Arial", 13, "bold"),
                                                 fg="#454545", bg="#FFF4E0",
                                                 command=lambda: self.backToMenu(self.screen))
        buttonCancelTransaction.place(x=25, y=410)

    def backToMenu(self, currentScreen):
        currentScreen.destroy()
        self.__init__(self.rabbitClass)

    def checkPayment(self, previousValueAccount, charge, size):
        self.sendCoffeToQueue(size)
        currentValueAccount = charge.checkAccountValue()

        if currentValueAccount > previousValueAccount:
            messagebox.showinfo(title="Sucesso",
                                message="Pagamento processado com sucesso! Seu café já está na fila e será produzido")
            #self.sendCoffeToQueue(size)
            self.backToMenu(self.screen)
        else:
            messagebox.showerror(title="Pagamento não processado",
                                 message="O pagamento ainda não foi processado. Se ele já tiver sido efetuado, "
                                         "aguarde alguns segundos e confirme novamente.")

    def sendCoffeToQueue(self, coffeSize):
        self.rabbitClass.sendToQueue(coffeSize)


x = GUI(RabbitQueue())
