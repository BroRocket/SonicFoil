
import os

from typing import Any
from customtkinter import *
from tkinter import Menu, filedialog

from utilities import ui_messages
import sonicfoil_backend as SonicFoil

#from widgets.menu_bar import CTkMenuBar

#self.master is root!

class ProgramPage(CTkFrame):
    def __init__(self, master: CTk, **kwargs):
        super().__init__(master, **kwargs)

        self.grid_columnconfigure((0 , 1 , 2, 3, 4), weight=1)
        self.grid_rowconfigure((0, 1, 2), weight=1)
        
        #Configure bar
        self.menu = Menu(self.master)
        self.file_menu = Menu(self.menu, tearoff=False)
        self.menu.add_cascade(label='File', menu=self.file_menu)
        self.file_menu.add_command(label='New', command=None) # add command
        self.file_menu.add_command(label='Open...', command=None) # add command
        self.file_menu.add_separator()
        self.file_menu.add_command(label='Settings', command=None)
        self.file_menu.add_command(label='Exit', command=self.master.quit)
        helpmenu = Menu(self.menu, tearoff=False)
        self.menu.add_cascade(label='Help', menu=helpmenu)
        helpmenu.add_command(label='About', command=None) # add command
        helpmenu.add_command(label='Guide', command=None) # add command

        self.master.configure(menu=self.menu)

        #load airfoil frame
        self.load_airfoil_frm = CTkFrame(self)
        self.load_airfoil_frm.grid_columnconfigure((0 , 1 , 2), weight=1)
        self.load_airfoil_frm.grid_rowconfigure((0, 1, 2), weight=1)

        self.load_airfoil_title_lbl = CTkLabel(self.load_airfoil_frm, text="Load Airfoil", font=("Arial", 16), anchor="center")
        self.load_airfoil_file_lbl = CTkLabel(self.load_airfoil_frm, text="File:", font=("Arial", 12), anchor="e")
        self.load_airfoil_file_opt = CTkOptionMenu(self.load_airfoil_frm, font=("Arial", 12), values= ["..."] + self._get_available_projects())
        self.load_airfoil_browse_file_btn = CTkButton(self.load_airfoil_frm, font=("Arial", 12), command=self._browse_files)
        self.load_airfoil_load_btn = CTkButton(self.load_airfoil_frm, font=("Arial", 14), command=self._load_airfoil)
        self.load_airfoil_save_btn = CTkButton(self.load_airfoil_frm, font=("Arial", 14), command=self._save_airfoil)

        self.load_airfoil_title_lbl.grid(row=0, column=0, columnspan=3, padx=5, pady=5, sticky="nsew")
        self.load_airfoil_file_lbl.grid(row=1, column=0, padx=(5, 1), pady=5, sticky="nsew")
        self.load_airfoil_file_opt.grid(row=1, column=1, padx=1, pady=5)
        self.load_airfoil_browse_file_btn.grid(row=1, column=2, padx=(1, 5), pady=5)
        self.load_airfoil_load_btn.grid(row=2, column=0, padx=5, pady=5, sticky="nsew")
        self.load_airfoil_save_btn.grid(row=2, column=2, padx=5, pady=5, sticky="nsew")
        self.load_airfoil_file_opt.grid(row=1, column=1, padx=(5, 1), pady=5, sticky="nsew")


        self.load_airfoil_frm.grid(row=0, column=0, columnspan=2, padx=5, pady=5, sticky="nsew")

        # input frame
        self.inputs_frm = CTkFrame(self)


        self.inputs_frm.grid(row=1, column=0, rowspan=2, columnspan=2, padx=5, pady=5, sticky="nsew")

        # plotter frame (use tabview)
        self.plots_frm = CTkFrame(self)


        self.plots_frm.grid(row=0, column=2, rowspan=2, columnspan=3, padx=5, pady=5, sticky="nsew")

        # text output frame
        self.text_output_frm = CTkFrame(self)


        self.text_output_frm.grid(row=2, column=2, columnspan=3, padx=5, pady=5, sticky="nsew")

        # self.configure_bar = CTkFrame(self)
        # self.grid_columnconfigure((0 , 1 , 2, 3), weight=1)
        # self.grid_rowconfigure((0), weight=1)


        # self.configure_bar.grid(row=0, column=0, columnspan=4, padx=1, pady=1, sticky="nsew")

    def _get_available_projects(self) -> list:
        cur_dir = os.path.join(os.getcwd(), "Airfoil Files")
        if os.path.exists(cur_dir):
            available_files = []
            dir_items = os.scandir(cur_dir)
            for itm in dir_items:
                if os.path.splitext(itm.name)[1] == ".dat":
                    available_files.append(itm.name)
            return available_files if available_files else ["None"]
        else:
            os.makedirs(cur_dir)
        return ["None"]
    
    def _browse_files(self) -> None:
        filename = filedialog.askopenfilename(initialdir = "/", title = "Select a File", filetypes = (("Airfoil files", "*.dat*"),("all files", "*.*")))
        self.load_airfoil_file_opt.set(filename)
        
    def _load_airfoil(self) -> None:
        filename = self.load_airfoil_file_opt.get()
        if os.path.exists(filename):
            if filename.split(".")[1] != "dat":
                ui_messages.gui_error(f'Entered filename "{filename}" is not the proper ".dat" format')
                return
            self.Airfoil = SonicFoil.Airfoil(filename)
            self._update_airfoil_plot()
        elif os.path.exists(os.path.join(os.getcwd(), "Airfoil Files", filename)):
            self.Arifoil = SonicFoil.Airfoil(os.path.join(os.getcwd(), "Airfoil Files", filename))
            self._update_airfoil_plot()
        else:
            ui_messages.gui_error(f'Entered filename "{filename}" does not exist')

    def _save_airfoil(self) -> None:
        pass

    def _update_airfoil_plot(self):
        pass

