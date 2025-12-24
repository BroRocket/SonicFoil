from typing import Any, Tuple
from customtkinter import *

from pages import program, start, airfoil_editor

class Application(CTk):
    def __init__(self, Title: str = "SonicFoil", color_theme: str = "blue", apearance_mode: str = "dark", fg_color: str | Tuple[str] | None = None, **kwargs):
        super().__init__(fg_color, **kwargs)

        set_appearance_mode(apearance_mode) 
        set_default_color_theme(color_theme) 
        self.title(Title)
        self.start_page()
        self.mainloop()

    def __clear(self) -> None:
        for child in self.children.copy():
            self.children[child].destroy() 

    def start_page(self):
        self.__clear()
        self.page = start.StartPage(self, self.project_page)
        self.page.pack(padx=5, pady=5, fill="both", expand=True)

    def project_page(self) -> None:
        self.__clear()
        self.page = program.ProgramPage(self)
        self.page.pack(padx=5, pady=5, fill="both", expand=True)

    def airfoil_editor_page(self) -> None: 
        window = CTkToplevel()
        window.title("Airfoil Editor")
        page = airfoil_editor.AirfoilEditor(window)
        page.pack(padx=5, pady=5, fill="both", expand=True)

if __name__ == "__main__":
    Application()