#import "@preview/minerva-report-fcfm:0.2.2" as minerva

#let titulo = "Tarea 2"
#let subtitulo = [Árboles B y $B^epsilon$] 
#let tema = ""

#let departamento = minerva.departamentos.dcc
#let curso = "CC4102 - Diseño y Análisis de Algoritmos"

#let fechas = ( // diccionario de fechas, si la portada no soporta
  entrega: minerva.formato-fecha(datetime.today())
)

#let lugar = ""

#let autores = ("Matías Rojas")
#let equipo-docente = (
  Profesores: ("Benjamín Bustos, 
Gonzalo Navarro") 
)
