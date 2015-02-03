void PatRec()
{
  TControlBar *bar2 = new TControlBar("vertical");
  bar2->AddButton("Residuals",".x ../macros/Residuals.C", "...  >>Residuals<<");
  bar2->AddButton("Pulls",".x ../macros/Pulls.C", "...  >>Pulls<<");
  bar2->AddButton("Efficiency",".x ../../tracking/macros/Efficiency.C", "...  >>Efficiency<<");
  bar2->AddButton("Bad Tracks",".x ../macros/BadTracks.C", "...  >>Bad Tracks<<");
  bar2->AddButton("Exit",".q", "...  >>Exit<<");
  bar2->Show();
}
