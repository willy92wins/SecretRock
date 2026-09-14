// Hologram overrides actually shipped (SR-01 — not "three methods only"):
//   ProjectionBasedOnParent / GetProjectionName / PlaceEntity
//     kit projects SecretRock_Holo_* (vanilla hologramMaterial ghost).
//     Spawn / GetDeployedClassname stays Land_LF_*. Never PlaceEntity the
//     Holo class (that would ECE_PLACE_ON_SURFACE a painted house).
//   SetProjectionPosition
//     terrain snap (sky clip) + Kit B look-push 1 m
//   GetDefaultOrientation
//     kit orientation offset (0 on both rocks)
//   IsColliding / EvaluateCollision / IsCollidingAngle / IsFloating
//     PowerGrid kit flags. Vanilla EvaluateCollision sets colliding on a
//     terrain-sized rock (IsInTerrain / BBox / angle) and ActionDeployObject
//     then hides place (ActionCondition needs !IsColliding).
//   RefreshVisual
//     super on the Holo_* projection only (wooden_case_deployable). Must
//     not run against Land_LF_* — GetHiddenSelection("inventory") is 0 and
//     would paint the whole placed mesh.
modded class Hologram
{
    protected const float SR_HOLO_GROUND_RAY_UP = 2.0;
    protected const float SR_HOLO_GROUND_RAY_DOWN = 20.0;
    protected const float SR_HOLO_GROUND_SNAP_Y = 0.05;

    protected bool SecretRock_IsKitProjection()
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                return true;
            }
        }

        return false;
    }

    protected vector SecretRock_SnapToTerrain(vector pos)
    {
        vector rayFrom = Vector(pos[0], pos[1] + SR_HOLO_GROUND_RAY_UP, pos[2]);
        vector rayTo = Vector(pos[0], pos[1] - SR_HOLO_GROUND_RAY_DOWN, pos[2]);
        vector groundHitPos;
        vector groundHitNormal;
        int groundComponent;
        set<Object> groundResults = null;
        Object gWith = m_Player;
        Object gIgnore = m_Projection;
        bool gSorted = false;
        bool gGroundOnly = false;
        float gRadius = 0.0;
        bool groundHit = DayZPhysics.RaycastRV(rayFrom, rayTo, groundHitPos, groundHitNormal, groundComponent, groundResults, gWith, gIgnore, gSorted, gGroundOnly, ObjIntersectFire, gRadius);
        if (groundHit)
        {
            vector snappedHit = Vector(groundHitPos[0], groundHitPos[1] + SR_HOLO_GROUND_SNAP_Y, groundHitPos[2]);
            return snappedHit;
        }

        float surfaceY = g_Game.SurfaceY(pos[0], pos[2]);
        vector snappedSurface = Vector(pos[0], surfaceY + SR_HOLO_GROUND_SNAP_Y, pos[2]);
        return snappedSurface;
    }

    override string ProjectionBasedOnParent()
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                string holoClass = deployKit.SecretRock_GetHologramClassname();
                if (holoClass != "")
                {
                    return holoClass;
                }
            }
        }

        return super.ProjectionBasedOnParent();
    }

    override string GetProjectionName(ItemBase item)
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                string holoName = deployKit.SecretRock_GetHologramClassname();
                if (holoName != "")
                {
                    return holoName;
                }
            }
        }

        return super.GetProjectionName(item);
    }

    override EntityAI PlaceEntity(EntityAI entity_for_placing)
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                // Kit, not the Holo_* projection. Vanilla PlaceEntity for
                // IsBasebuildingKit already returns the kit; do not
                // ECE_OBJECT_SWAP / ECE_PLACE_ON_SURFACE the hologram class.
                return entity_for_placing;
            }
        }

        return super.PlaceEntity(entity_for_placing);
    }

    override void SetProjectionPosition(vector position)
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                vector depOffset = deployKit.GetDeployPositionOffset();
                vector depFinal = position + depOffset;
                float lookPushM = deployKit.SecretRock_GetHologramLookPushMeters();
                if (lookPushM > 0.0 && m_Player)
                {
                    vector look = m_Player.GetDirection();
                    look[1] = 0;
                    float lookLen = look.Length();
                    if (lookLen > 0.001)
                    {
                        look = look * (1.0 / lookLen);
                        vector lookPush = look * lookPushM;
                        depFinal = depFinal + lookPush;
                    }
                }
                vector snapped = SecretRock_SnapToTerrain(depFinal);

                if (m_Projection)
                {
                    m_Projection.SetPosition(snapped);
                }
                return;
            }
        }

        super.SetProjectionPosition(position);
    }

    override vector GetDefaultOrientation()
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                vector baseOri = super.GetDefaultOrientation();
                vector oriOff = deployKit.GetDeployOrientationOffset();
                vector oriResult = baseOri + oriOff;
                return oriResult;
            }
        }

        return super.GetDefaultOrientation();
    }

    override bool IsColliding()
    {
        if (SecretRock_IsKitProjection())
        {
            return false;
        }
        return super.IsColliding();
    }

    override void EvaluateCollision(ItemBase action_item)
    {
        if (SecretRock_IsKitProjection())
        {
            bool bNoCollide = false;
            SetIsColliding(bNoCollide);
            return;
        }
        super.EvaluateCollision(action_item);
    }

    override bool IsCollidingAngle()
    {
        if (SecretRock_IsKitProjection())
        {
            return false;
        }
        return super.IsCollidingAngle();
    }

    override bool IsFloating()
    {
        if (SecretRock_IsKitProjection())
        {
            return false;
        }
        return super.IsFloating();
    }

    override void RefreshVisual()
    {
        if (SecretRock_IsKitProjection())
        {
            // Holo_* proxy: vanilla wooden_case ghost. Do not skip — a
            // no-op leaves native rock/concrete on the projection.
            super.RefreshVisual();
            return;
        }
        super.RefreshVisual();
    }
};
