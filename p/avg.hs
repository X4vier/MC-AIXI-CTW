main = do
   (_:ls) <- getContents >>= return.lines
   let 
      g (c:_:r:_) = (c,r)
      f [] = []
      f xs = (fst (head ys),fromIntegral (foldl1 (+) (map snd ys)) / fromIntegral (length ys)) : f xs'
         where
         (ys,xs') = splitAt 1000 xs
      xs = map (g.map (read.takeWhile(/=',')).words) ls
   putStr $ unlines $ ("cycle,winavg":) $ map (\(a,b)->show a++","++show b) (reverse (f (reverse xs))::[(Int,Double)])
