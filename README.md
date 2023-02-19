# INFO804 - TP2
<img src="./Selection/output_19-02-2023 20-57-41.png">

## Lancement

`./ray-tracing` -> Lance l'application

Dans l'application plusieurs raccourcis proposés dans la fenêtre d'aide

Raccourcis supplémentaires : 

- R = rendu classique, J = rendu avec anti-aliasing
- Si Ctrl / Maj appuyé, décuple la résolution pour les deux raccourcis au-dessus.
- D / Maj + D = augmente/diminue la profondeur du rayon.

## Bêtisier

<img src="./Selection/output_14-02-2023 11-46-07.png">
<img src="./Selection/output_14-02-2023 11-51-09.png">

**_Les plans infinis d'eau vont s'intersecter avec l'infini_**

<img src="./Selection/output_15-02-2023 11-10-43.png">

**_Et la... il vient se courber sur lui-même_**

<img src="./Selection/output_15-02-2023 12-10-05.png">

**_Ici, les vagues sont bien trop grandes par rapport au reste des objets de la scène_**

<img src="./Selection/output_15-02-2023 12-11-05.png">
<img src="./Selection/output_15-02-2023 12-36-21.png">

**_Et la, en plus de l'aliasing, l'eau mange littéralement les sphères alors que celles-ci sont bien au dessus dans la simulation_**

<img src="./Selection/output_15-02-2023 12-29-18.png">

**_L'eau opaque qui s'auto-aliasing avec ses propres contours_**


<img src="./Selection/output_15-02-2023 12-41-45.png">
<img src="./Selection/output_15-02-2023 16-18-48.png">

**_Ici les vagues sont si petites que des patterns de moiré apparaissent_**

<img src="./Selection/output_15-02-2023 16-21-17.png">

**_Mauvaise réflexion des rayons, qui viennent prendre la couleur au carré des surfaces touchées_**

<img src="./Selection/output_15-02-2023 16-43-26.png">

**_Ombrage non cohérent : la lumière est à gauche de la scène_**

<img src="./Selection/output_15-02-2023 16-43-46.png">

**_Et la... le ciel est trop fortement réfléchi et trop assombri dans les sphères_**

## Belles images

<img src="./Selection/output_14-02-2023 10-12-12.png">
<img src="./Selection/output_14-02-2023 10-22-20.png">

**_Première réussite d'ombrages avec réflexions correctes_**

<img src="./Selection/output_14-02-2023 10-29-11.png">

**_La bulle semble découper la sphère d'émeraude en deux_**

<img src="./Selection/output_14-02-2023 11-10-01.png">

**_Le building fini / infini dans un monde surréaliste_**

<img src="./Selection/output_15-02-2023 17-56-48.png">

**_Rappelez-vous : protégez vos sphères lors de vos voyages en égypte, surtout près des pyramides d'eau_**

<img src="./Selection/output_16-02-2023 17-10-10.png">

**_Les anneaux de la destinées (Prototype commenté dans [ray-tracer](ray-tracer.cpp) sous le nom "Image surréaliste")_**

<img src="./Selection/output_18-02-2023 23-20-19.png">

**_Blblblblblbl..._**

<img src="./Selection/output_19-02-2023 01-32-55.png">

**_Isekai no Pyramido (Prototype commenté dans [ray-tracer](ray-tracer.cpp) sous le nom "Isekai no Pyramido")_**

<img src="./Selection/output_19-02-2023 20-57-41.png">

**_Isekai no Pyramido : Kami no Kyu (Prototype commenté dans [ray-tracer](ray-tracer.cpp) sous le nom "Cover d'album futuriste")_**