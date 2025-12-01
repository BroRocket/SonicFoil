from typing import Any
from customtkinter import *
import os

from utilities import ui_messages

class StartPage(CTkFrame):
    def __init__(self, master: CTk, func: Any, **kwargs):
        super().__init__(master, **kwargs)
        self.func = func
        #self.load_project_func = load_func

        self.grid_columnconfigure((0), weight=1)
        self.grid_rowconfigure((0, 1, 2), weight=1)

        title_frm = CTkFrame(self)
        self.title = CTkLabel(title_frm, text="Welcome To SonicFoil", text_color="lightblue", font=("Arial", 30), anchor="center")
        self.title.pack(pady=20, fill="both", expand=True)

        notice_frm = CTkFrame(self)
        notice_frm.grid_columnconfigure((0), weight=1)
        notice_frm.grid_rowconfigure((0, 1), weight=1)
        self.notice_label = CTkLabel(notice_frm, text="This software allows you to simulate supersonic flow over a an airfoil.\nIt allows for the use of the ackeret and wave/shock mehtods and approximating skin friction.\nTHe tool is free use and produced by Brody Howard.\n Click Ok to start the program.", font=("Arial", 16), anchor="center")
        self.notice_label.grid(row=0, column=0, padx=(10, 10), pady=(10, 10), sticky="ew")
        self.load_btn = CTkButton(notice_frm, text="OK", font=("Arial", 18), anchor="center", command=self.load_program)
        self.load_btn.grid(row=1, column=0, padx=(10, 10), pady=(5, 15), sticky="ew")

        title_frm.grid(row=0, column=0, padx=5, pady=5, sticky="nsew")
        notice_frm.grid(row=1, rowspan=2, column=0, columnspan=2, padx=5, pady=5, sticky="nsew")

    def load_program(self) -> None:
        self.func()