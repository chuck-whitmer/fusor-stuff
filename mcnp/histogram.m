(* ::Package:: *)

(* ::Input::Initialization:: *)
BeginPackage["histogram`"];

histogram`Private`myNames={"getCsvTallyFromClipboard","getMcnpTallyFromClipboard","linearFluenceHist","linearFluenceHistWithTitle","logFluenceHist","logFluenceHistWithTitle"};
Unprotect @@ Names["histogram`*"];
ClearAll @@ histogram`Private`myNames;

 getMcnpTallyFromClipboard::usage = "getMcnpTallyFromClipboard[] - Get MCNP tally data directly from clipboard";
  getCsvTallyFromClipboard::usage = "getCsvTallyFromClipboard[] - Get two columns of data directly from clipboard";
  linearFluenceHistWithTitle::usage = "linearFluenceHistWithTitle[tally,title]. References xRange and yRange.";
  linearFluenceHist::usage="linearFluenceHist[tally]. References xRange and yRange.";
  logFluenceHistWithTitle::usage="logFluenceHistWithTitle[tally,title]. References xRange and yRange.";
  logFluenceHist::usage="logFluenceHist[footally]. References xRange and yRange.";

Begin["`Private`"];
getMcnpTallyFromClipboard[]:=Module[{eList,nList,errList,toPlot},{eList,nList,errList}=Transpose[ImportString[ NotebookGet[ClipboardNotebook[]][[1,1,1]],"Table"]];toPlot = Transpose[{10^6 Drop[eList,-1],Drop[nList,1]}];{eList,nList,errList,toPlot}];
getCsvTallyFromClipboard[]:=Module[{eList,nList,errList,toPlot},{eList,nList}=Transpose[ImportString[ NotebookGet[ClipboardNotebook[]][[1,1,1]],"CSV"]];toPlot = Transpose[{Drop[eList,-1],Drop[nList,1]}];{eList,nList,errList,toPlot}];
linearFluenceHistWithTitle[tally_,title_]:=Module[{eList,nList,errList,toPlot},{eList,nList,errList,toPlot}=tally;ListStepPlot[toPlot,PlotRange->{Global`xRange,Global`yRange},
AxesLabel->{"eV","(n/\!\(\*SuperscriptBox[\(cm\), \(2\)]\))/src"},Filling->Axis,PlotLabel->title<>"
Total = "<>ToString[Total[nList],TraditionalForm]<>"(n/\!\(\*SuperscriptBox[\(cm\), \(2\)]\))/src"]];
linearFluenceHist[tally_]:=Module[{eList,nList,errList,toPlot},{eList,nList,errList,toPlot}=tally;ListStepPlot[toPlot,PlotRange->{Global`xRange,Global`yRange},
AxesLabel->{"eV","(n/\!\(\*SuperscriptBox[\(cm\), \(2\)]\))/src"},Filling->Axis,PlotStyle->Red]];
logFluenceHistWithTitle[tally_,title_]:=Module[{eList,nList,errList,toPlot},{eList,nList,errList,toPlot}=tally;ListStepPlot[toPlot,PlotRange->{Global`xRange,Global`yRange},ScalingFunctions->{"Log10"},
AxesLabel->{"eV","(n/\!\(\*SuperscriptBox[\(cm\), \(2\)]\))/src"},Ticks->{Global`myTicks,Automatic},Filling->Axis,PlotLabel->title<>"
Total = "<>ToString[Total[nList],TraditionalForm]<>"(n/\!\(\*SuperscriptBox[\(cm\), \(2\)]\))/src"]];
logFluenceHist[tally_]:=Module[{eList,nList,errList,toPlot},{eList,nList,errList,toPlot}=tally;ListStepPlot[toPlot,PlotRange->{Global`xRange,Global`yRange},ScalingFunctions->{"Log10"},
AxesLabel->{"eV","(n/\!\(\*SuperscriptBox[\(cm\), \(2\)]\))/src"},Ticks->{Global`myTicks,Automatic},Filling->Axis,PlotStyle->Red]];
End[];
Protect @@ histogram`Private`myNames;
EndPackage[];
Global`myTicks={10^#,Superscript[10,#]}&/@Range[-14,7];
Global`xRange=All; Global`yRange=All;
Print["histogram package loaded - You will want to reset xRange and yRange"];
