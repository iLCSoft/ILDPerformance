## To check the simulation SimCalorimeterHit stepLength

### after simulation, one could run the following Marlin job to generate the stepLength root tree file with his simulation input.
 
```shell
Marlin StepLength.xml \
  --global.LCIOInputFiles=MySimulation_step_length_max_5.0_mm_10evts_SIM.slcio \
  --MyAIDAProcessor.FileName=stepLength
```

```shell
root stepLength.root
StepLength->Draw("stepLength", "PDG==11")
```
