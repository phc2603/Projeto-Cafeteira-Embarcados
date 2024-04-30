import tkinter
from tkinter import messagebox

COR_DE_FUNDO = "#967259"


class GUI:
    def __init__(self):
        self.screen = tkinter.Tk()
        self.screen.title("Cafeteira Embarcados")
        self.screen.config(padx=5, pady=9, bg=COR_DE_FUNDO)
        self.screen.geometry("800x500")

        label_title = tkinter.Label(text='Escolha o tamanho do café', font=("Arial", 20, "bold"), fg="#ece0d1",
                                    bg=COR_DE_FUNDO)
        label_title.place(x=200, y=50)

        smallCoffeImage = tkinter.PhotoImage(file="cafeP.png")

        self.smallCoffeButton = tkinter.Button(self.screen, image=smallCoffeImage, fg="#454545", bg="#FFF4E0")
        self.smallCoffeButton.place(x=35, y=200)
        label_small = tkinter.Label(text='P', font=("Arial", 20, "bold"), fg="#ece0d1",
                                    bg=COR_DE_FUNDO)
        label_small.place(x=100, y=360)

        self.mediumCoffeButton = tkinter.Button(self.screen, image=smallCoffeImage, fg="#454545", bg="#FFF4E0")
        self.mediumCoffeButton.place(x=300, y=200)
        label_medium = tkinter.Label(text='M', font=("Arial", 20, "bold"), fg="#ece0d1",
                                     bg=COR_DE_FUNDO)
        label_medium.place(x=365, y=360)

        self.largeCoffeButton = tkinter.Button(self.screen, image=smallCoffeImage, fg="#454545", bg="#FFF4E0")
        self.largeCoffeButton.place(x=560, y=200)
        label_large = tkinter.Label(text='G', font=("Arial", 20, "bold"), fg="#ece0d1",
                                    bg=COR_DE_FUNDO)
        label_large.place(x=625, y=360)

        self.screen.mainloop()


x = GUI()
