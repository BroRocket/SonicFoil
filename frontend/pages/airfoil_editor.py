
import os
from customtkinter import *
from tkinter import Menu, filedialog, StringVar
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

from utilities import ui_messages
from widgets.airfoil_data_entry import CTkAirfoilDataEntry
from widgets.plot import CTkPlot
import sonicfoil_backend as SonicFoil


def new_airfoil_editor():
    window = CTkToplevel()
    window.title("Airfoil Editor")
    page = AirfoilEditor(window)
    page.pack(padx=5, pady=5, fill="both", expand=True)


class AirfoilEditor(CTkFrame):
   def __init__(self, master: CTk, **kwargs):
      super().__init__(master, **kwargs)

      # add menu like program has so you can load in an airofil if prefered.
      # otherwise let them choose an airofil name, then set the points using entry widgets and alow for deleting points. automatically update the plot with points. 
      # Try to maybe have editing through the plot somehow.

      self.grid_columnconfigure((0 , 1), weight=1)
      self.grid_rowconfigure((0, 1, 2, 3), weight=1)
      
      #Configure bar
      self.menu = Menu(self.master)
      self.file_menu = Menu(self.menu, tearoff=False)
      self.menu.add_cascade(label='File', menu=self.file_menu)
      self.file_menu.add_command(label='New', command=new_airfoil_editor) 
      self.file_menu.add_command(label='Open...', command=self.open_airfoil) # add command
      self.file_menu.add_separator()
      self.file_menu.add_command(label='Settings', command=None) # add command
      self.file_menu.add_command(label='Exit', command=self.master.quit)
      helpmenu = Menu(self.menu, tearoff=False)
      self.menu.add_cascade(label='Help', menu=helpmenu)
      helpmenu.add_command(label='About', command=None) # add command
      helpmenu.add_command(label='Guide', command=None) # add command
      
      self.master.configure(menu=self.menu)

      ######### Name Frame #########

      self.airfoil_name_frm = CTkFrame(self)
      self.airfoil_name_frm.grid_columnconfigure((0, 1), weight=1)
      self.airfoil_name_frm.grid_rowconfigure(0, weight=1)

      self.airfoil_name_txt = CTkLabel(self.airfoil_name_frm, text="Airfoil Name:", font=("Arial", 16), anchor="center")
      self.AirfoilNameVAR = StringVar(value="")
      self.airfoil_name_ent = CTkEntry(self.airfoil_name_frm, textvariable=self.AirfoilNameVAR)

      self.airfoil_name_txt.grid(row=0, column=0, padx=5, pady=5, sticky="ew")
      self.airfoil_name_ent.grid(row=0, column=1, padx=5, pady=5, sticky="ew")

      self.airfoil_name_frm.grid(row=0, column=0, padx=10, pady=10, sticky="nswe")

      ##############################

      ######### Plot Frame #########

      self.plots_frm = CTkFrame(self)
      self.plots_frm.grid_columnconfigure(0, weight=1)
      self.plots_frm.grid_rowconfigure(0, weight=1)

      self.airfoil_plot = CTkPlot(self.plots_frm, "Airfoil Schematic", "x/c", "y/c", dpi=150)

      self.airfoil_plot.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

      self.plots_frm.grid(row=0, column=1, rowspan=3, padx=10, pady=10, sticky="nsew")

      ##############################

      ######### Points Frame #######

      self.points_frm = CTkFrame(self)
      self.points_frm.grid_columnconfigure(0, weight=1)
      self.points_frm.grid_rowconfigure(0, weight=0)
      self.points_frm.grid_rowconfigure(1, weight=1)

      self.title_lbl = CTkLabel(self.points_frm, text="Airfoil Points", font=("Arial", 18), anchor="center")
      self.points = CTkAirfoilDataEntry(self.points_frm, self.airfoil_plot) # add plot

      self.title_lbl.grid(row=0, column=0, padx=5, pady=5, sticky="nsew")
      self.points.grid(row=1, column=0, padx=5, pady=5, sticky="nsew")

      self.points_frm.grid(row=1, column=0, rowspan=3, padx=10, pady=10, sticky="nsew")

      ##############################

      ### Setup Plot CLicks
      self.airfoil_plot.canvas.mpl_connect("button_press_event", self._on_plot_click)
      ######

      ######### Save Frame #########

      self.save_frm = CTkFrame(self)
      self.save_frm.grid_columnconfigure(0, weight=1)
      self.save_frm.grid_rowconfigure(0, weight=1)

      self.save_button = CTkButton(self.save_frm, text="Save Airfoil", command=self._save_airfoil_file) ## wrtie this function
      
      self.save_button.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

      self.save_frm.grid(row=3, column=1, padx=10, pady=10, sticky="nsew")

      ##############################

   def _on_plot_click(self, event):
      # Only react to left-clicks inside the axes
      if event.inaxes != self.airfoil_plot.ax:
         return
      if event.xdata is None or event.ydata is None:
         return
      if event.button != 1:  # 1 = left mouse button
         return

      x = event.xdata
      y = event.ydata

      # Call your existing API: pos=None means "decide insert index from x/y"
      self.points._add_point(pos=None, x=str(x), y=str(y))

   def open_airfoil(self):
      #open existing airfoil file
      filename = filedialog.askopenfilename(initialdir = os.path.join(os.getcwd(), "Airfoil Files"), title = "Select a File", filetypes = (("Airfoil files", "*.dat*"),("all files", "*.*")))
      if not filename.endswith(".dat"):
         ui_messages.gui_error(f'Entered filename "{filename}" is not the proper ".dat" format')
         return
      
      try:
         airfoil = SonicFoil.Airfoil(filename)
      except:
         ui_messages.gui_error(f'Aifoil File "{filename}" is not in correct format to be loaded in.')
         return
      
      self.AirfoilNameVAR.set(airfoil.name)
      
      x = []
      y = []
      for segment in reversed(airfoil.top_segments):
         x.append(segment.end.x)
         y.append(segment.end.y)
      for segment in airfoil.bottom_segments:
         x.append(segment.start.x)
         y.append(segment.start.y)
      x.append(segment.end.x)
      y.append(segment.end.y)
     
      self.points.load_in_airfoil(x, y)    

   def _save_airfoil_file(self):
      airfoil_name = self.AirfoilNameVAR.get()
      if airfoil_name == "":
         ui_messages.gui_error("Please enter an airfoil name before saving.")
         return
      
      # do some airfoil validation.
      self.points.update_points()
      points = self.points.get_points()
      if len(points) < 3:
         ui_messages.gui_error("Not enough points please add more points before saving to make a valid airfoil.")
         return
      if points[0] != points[-1]:
         ui_messages.gui_error("The first and last point must be the same to close the airfoil shape.")
         return
      found_origin = False
      for point in points:
         if point[0] == 0 and point[1] == 0:
            found_origin = True
      if not found_origin:
         ui_messages.gui_error("Airfoil must include the origin (0, 0) as the 'middle' point which seperates top and bottom of airfoil.")
         return

      filename = filedialog.asksaveasfilename(defaultextension=".dat", initialdir = os.path.join(os.getcwd(), "Airfoil Files"), title = "Select a File", filetypes = (("Airfoil files", "*.dat*"),("all files", "*.*")))
      if not filename:
         return
      
      try:
        with open(filename, "w") as f:
            f.write(f"{airfoil_name}\n")
            for x, y in points:
               f.write(f"{x:.6f} {y:.6f}\n")
      except Exception as e:
         ui_messages.gui_error(f"Failed to save airfoil:\n{e}")

      #add pop up for sucess

      







