# EstacionPrecalentamiento
Esta es una estación de precalemtamiento para soldar componentes SMD en general
Está preparado para poder funcionar de 50 grados a 250 grados. 
Dependiendo de la placa calentadora que se compre, puede que haya que volver a ajustar los valores del PID.


Componentes utilizados:
- Arduino NANO
- Placa calefactora PTC 500W 20cm x 10cm
- Termistor NTC 100K 3950
- Resistencia PULLUP de 4K7 para el termistor
- Condensadores de 10nF (irá en paralelo con la resistencia de 4K7)
- Condensador de 22uF/16V (irá en paralelo con la resistencia de 4K7)
- Display LCD2004 con placa adaptadora I2C
- SSR10-DA (10A)
- Cargador de móvil (esto será para sacar la placa y alimentar al arduino NANO y LCD


Los tornillos que van puestos en la placa calentadora, se calientan bastante, así que la mejor solución ha sido poner dos trozos de madera como placa intermedia.
