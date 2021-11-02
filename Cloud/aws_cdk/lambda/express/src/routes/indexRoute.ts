import { Router } from "express";

import PairRouter from "./pairRoute";
import UsrRouter from "./userRoute";

// Init router and path
const router = Router();

// Add sub-routes
router.use("/user", UsrRouter);
router.use("/pairing", PairRouter);
// Export the base-router
export default router;
