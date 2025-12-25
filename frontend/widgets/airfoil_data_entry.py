from customtkinter import *

from frontend.widgets.plot import CTkPlot

class CTkAirfoilDataEntry(CTkScrollableFrame):
    def __init__(self, master: CTk, plot: CTkPlot, **kwargs):
        super().__init__(master, **kwargs)

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure((0, 1, 2), weight=1)

        self.plot = plot
        self.points = [(1, 0), (0, 0), (1, 0)]
        self.point_entries = []

        point1 = CTkPointsEntry(self, self._add_point, self._remove_point, self.update_points, 0, "1", "0")
        self.point_entries.append(point1)

        point2 = CTkPointsEntry(self, self._add_point, self._remove_point, self.update_points, 1, "0", "0")
        self.point_entries.append(point2)
        
        point3 = CTkPointsEntry(self, self._add_point, self._remove_point, self.update_points, 2, "1", "0")
        self.point_entries.append(point3)

        point1.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")
        point2.grid(row=1, column=0, padx=10, pady=10, sticky="nsew")
        point3.grid(row=2, column=0, padx=10, pady=10, sticky="nsew")

        self.update_points()

    def update_points(self) -> None:
        # update points list and then update plot?
        temp_points = [pe.get_point() for pe in self.point_entries]
        self.points = [p for p in temp_points if p != (None, None)]        
        # update graph? Handle None value for new point
        self.plot.clear()
        x = []
        y = []
        for point in self.points:
            x.append(point[0])
            y.append(point[1])
        
        if x and y:
            self.plot.add_line(x, y, "_nolegend_", color="k")
            self.plot.ax.set_aspect('equal', adjustable='datalim')

    def _add_point(self, pos: int|None, x: str = "", y: str = "") -> None:

        if pos is None: # solely handles case of being fed point from plot click
            self.update_points()
            try:
                x_val = float(x)
                y_val = float(y)
            except ValueError:
                return  # bad click data, bail

            if y_val >= 0:
                for i in range(len(self.points) - 1):
                    if x_val < self.points[i][0] and x_val > self.points[i+1][0]:
                        pos = i + 1
                        break
            else:
                for i in range(len(self.points) - 1, 0, -1):
                    if x_val < self.points[i][0] and x_val > self.points[i-1][0]:
                        pos = i
                        break
            x = x[:6]
            y = y[:6]
        if pos is None:
            pos = len(self.point_entries)
             
        point = CTkPointsEntry(self, self._add_point, self._remove_point, self.update_points, pos, x, y)

        if pos < len(self.point_entries):
            self.point_entries.insert(pos, point)
            self.grid_rowconfigure(tuple(range(len(self.point_entries))), weight=1)
            point.grid(row=pos, column=0, padx=10, pady=10, sticky="nsew")
            ind = pos + 1
            for point_entry in self.point_entries[ind:]:
                point_entry.ind = ind
                point_entry.grid(row=ind, column=0, padx=10, pady=10, sticky="nsew")
                ind += 1
        else:
            self.point_entries.append(point)
            self.grid_rowconfigure(tuple(range(len(self.point_entries))), weight=1)
            point.grid(row=pos, column=0, padx=10, pady=10, sticky="nsew")

        self.update_points()

    def _remove_point(self, pos: int) -> None:
        if len(self.point_entries) <= 1:
            return

        entry = self.point_entries.pop(pos)
        entry.destroy()
        self.grid_rowconfigure(tuple(range(len(self.point_entries))), weight=1)

        i = pos
        for point_entry in self.point_entries[pos:]:
            point_entry.ind = i
            point_entry.grid(row=i, column=0, padx=10, pady=10, sticky="nsew")
            i += 1

        self.update_points()
        
    def get_points(self) -> list[tuple]:
        self.update_points()
        return self.points
    
    def load_in_airfoil(self, xs: list[float], ys: list[float]) -> None:
        self.point_entries.clear()
        i = 0
        for x, y in zip(xs, ys):
            point = CTkPointsEntry(self, self._add_point, self._remove_point, self.update_points, i, str(x), str(y))
            self.point_entries.append(point)
            self.grid_rowconfigure(i, weight=1)
            point.grid(row=i, column=0, padx=10, pady=10, sticky="nsew")
            i += 1
        self.update_points()
        

class CTkPointsEntry(CTkFrame):
    def __init__(self, master: CTk, add_command, remove_command, update_command, ind: int, x: str = "", y: str = "", **kwargs):
        super().__init__(master, **kwargs)

        self.grid_columnconfigure(0, weight=1)  # x entry
        self.grid_columnconfigure(1, weight=1)  # y entry
        self.grid_columnconfigure(2, weight=0)
        self.grid_columnconfigure(3, weight=0)
        self.grid_rowconfigure(0, weight=1)

        self.ind = ind

        self.x_var = StringVar(value=x)
        self.y_var = StringVar(value=y)

        self.x_ent = CTkEntry(self, textvariable=self.x_var)
        self.y_ent = CTkEntry(self, textvariable=self.y_var)

        self.add_button = CTkButton(self, text="Add", command=lambda : add_command(self.ind+1), text_color="white", hover_color="dark green", fg_color="green", anchor="center")
        self.remove_button = CTkButton(self, text="Delete", command=lambda : remove_command(self.ind), text_color="white", hover_color="dark red", fg_color="red", anchor="center")

        self.x_ent.grid(row=0, column=0, padx=(10, 5), pady=10, sticky="nsew")
        self.y_ent.grid(row=0, column=1, padx=(5, 10), pady=10, sticky="nsew")
        self.remove_button.grid(row=0, column=2, padx=(10, 5), pady=10, sticky="nsew")
        self.add_button.grid(row=0, column=3, padx=(5, 10), pady=10, sticky="nsew")

        self.x_ent.bind("<Return>", lambda event : update_command())
        self.y_ent.bind("<Return>", lambda event : update_command())

    def get_point(self):
        try:
            return (float(self.x_var.get().strip()), float(self.y_var.get().strip())) if self.x_var.get() != "" and self.y_var.get() != "" else (None, None)
        except ValueError as e:
            return (None, None)



        