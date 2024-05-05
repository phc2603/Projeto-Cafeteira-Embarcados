import tkinter
from apiMain import ApiMain
from tkinter import messagebox

COR_DE_FUNDO = "#967259"


class GUI:
    def __init__(self):
        self.screen = tkinter.Tk()
        self.screen.title("Cafeteira Embarcados")
        self.screen.config(padx=5, pady=9, bg=COR_DE_FUNDO)
        self.screen.geometry("800x500")
        self.qrCodeImage = None

        label_title = tkinter.Label(text='Escolha o tamanho do café', font=("Arial", 20, "bold"), fg="#ece0d1",
                                    bg=COR_DE_FUNDO)
        label_title.place(x=200, y=50)

        smallCoffeImage = tkinter.PhotoImage(file="small.png")

        self.smallCoffeButton = tkinter.Button(self.screen, image=smallCoffeImage, fg="#454545", bg="#FFF4E0",
                                               command=lambda: self.changeScreenToQrCode("1.00"))
        self.smallCoffeButton.place(x=75, y=225)
        label_small = tkinter.Label(text='S', font=("Arial", 20, "bold"), fg="#ece0d1",
                                    bg=COR_DE_FUNDO)
        label_small.place(x=100, y=360)

        mediumCoffeImage = tkinter.PhotoImage(file="medium.png")
        self.mediumCoffeButton = tkinter.Button(self.screen, image=mediumCoffeImage, fg="#454545", bg="#FFF4E0",
                                                command=lambda: self.changeScreenToQrCode("3.00"))
        self.mediumCoffeButton.place(x=335, y=215)
        label_medium = tkinter.Label(text='M', font=("Arial", 20, "bold"), fg="#ece0d1",
                                     bg=COR_DE_FUNDO)
        label_medium.place(x=365, y=360)

        largeCoffeImage = tkinter.PhotoImage(file="large.png")
        self.largeCoffeButton = tkinter.Button(self.screen, image=largeCoffeImage, fg="#454545", bg="#FFF4E0",
                                               command=lambda: self.changeScreenToQrCode("5.00"))
        self.largeCoffeButton.place(x=585, y=205)
        label_large = tkinter.Label(text='L', font=("Arial", 20, "bold"), fg="#ece0d1",
                                    bg=COR_DE_FUNDO)
        label_large.place(x=625, y=360)

        self.screen.mainloop()

    def changeScreenToQrCode(self, coffeValue):
        charge = ApiMain()
        previousValueAccount = charge.checkAccountValue()
        currentCharge = charge.generateCharge(coffeValue)
        if currentCharge:
            self.screen.destroy()
            qrCodeScreen = tkinter.Tk()
            qrCodeScreen.title("Pagamento")
            qrCodeScreen.config(padx=5, pady=9, background=COR_DE_FUNDO)
            qrCodeScreen.geometry("800x800")

            label_instructions = tkinter.Label(text="Realize o pagamento no app bancário e aguarde para seu café "
                                                    "começará a ser"
                                                    "preparado", font=("Arial", 12, "bold"), fg="#ece0d1",
                                               bg=COR_DE_FUNDO)
            label_instructions.place(x=50, y=50)

            self.qrCodeImage = tkinter.PhotoImage(file="qrCodePagamentoCafe.png")
            panel = tkinter.Label(qrCodeScreen, image=self.qrCodeImage, fg="#454545", bg="#FFF4E0")
            panel.place(x=100, y=100)

            backMenuButton = tkinter.Button(qrCodeScreen, text="Cancelar e voltar ao menu", font=("Arial", 15, "bold"),
                                            fg="#454545", bg="#FFF4E0",
                                            command=lambda: self.backToMenu(qrCodeScreen))
            backMenuButton.place(x=25, y=750)

            checkPayment = tkinter.Button(qrCodeScreen, text="Realizei o pagamento", font=("Arial", 15, "bold"),
                                          fg="#454545", bg="#FFF4E0",
                                          command=lambda: self.checkPayment(qrCodeScreen, previousValueAccount, charge))
            checkPayment.place(x=550, y=750)

        else:
            messagebox.showerror(title="Error", message="Ocorreu um erro ao ir para o pagamento. Favor tente novamente mais tarde")

    def backToMenu(self, currentScreen):
        currentScreen.destroy()
        self.__init__()

    def checkPayment(self, currentScreen, previousValueAccount, charge):
        currentValueAccount = charge.checkAccountValue()
        if currentValueAccount > previousValueAccount:
            messagebox.showinfo(title="Sucesso", message="Pagamento processado com sucesso! Seu café será produzido agora")
            self.backToMenu(currentScreen)
        else:
            messagebox.showerror(title="Error", message="O pagamento ainda não foi processado. Aguarde mais alguns instantes e verifique "
                                 "novamente.")


x = GUI()
