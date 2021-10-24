import { Router } from "express";
import UsrRouter from "./usrRoute";

// Init router and path
const router = Router();

// Add sub-routes
router.use("/user", UsrRouter);

// Export the base-router
export default router;
