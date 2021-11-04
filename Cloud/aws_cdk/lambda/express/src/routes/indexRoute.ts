import { Router } from "express";

import PairRouter from "./pairRoute";

const router = Router();

router.use("/pairing", PairRouter);

export default router;
